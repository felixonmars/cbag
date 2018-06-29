#include <cbagoa/database.h>

namespace cbagoa {

    cbag::Orientation convert_orient(const oa::oaOrient &orient) {
        switch(orient) {
            case oa::oacR0:
                return cbag::Orientation::R0;
            case oa::oacR90:
                return cbag::Orientation::R90;
            case oa::oacR180:
                return cbag::Orientation::R180;
            case oa::oacR270:
                return cbag::Orientation::R270;
            case oa::oacMY:
                return cbag::Orientation::MY;
            case oa::oacMYR90:
                return cbag::Orientation::MYR90;
            case oa::oacMX:
                return cbag::Orientation::MX;
            case oa::oacMXR90:
                return cbag::Orientation::MXR90;
            default:
                throw std::invalid_argument("Unknown orientation code.");
        }
    }

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"

    oa::oaBoolean LibDefObserver::onLoadWarnings(oa::oaLibDefList *obj, const oa::oaString &msg,
                                                 oa::oaLibDefListWarningTypeEnum type) {
        std::string msg_std(msg);
        throw std::runtime_error("OA Error: " + msg_std);
    }

#pragma clang diagnostic pop

    void Library::open_lib(const std::string &lib_file, const std::string &library,
                           const std::string &lib_path, const std::string &tech_lib) {
        try {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
            oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                         oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

            // open library definition
            oa::oaString lib_def_file(lib_file.c_str());
            oa::oaLibDefList::openLibs(lib_def_file);

            // open library
            lib_name = library;
            lib_name_oa = oa::oaScalarName(ns, library.c_str());
            lib_ptr = oa::oaLib::find(lib_name_oa);
            if (lib_ptr == nullptr) {
                // create new library
                oa::oaString oa_lib_path(lib_path.c_str());
                oa::oaScalarName oa_tech_lib(ns, tech_lib.c_str());
                lib_ptr = oa::oaLib::create(lib_name_oa, oa_lib_path);
                oa::oaTech::attach(lib_ptr, oa_tech_lib);

                // NOTE: I cannot get open access to modify the library file, so
                // we just do it by hand.
                std::ofstream outfile;
                outfile.open(lib_file, std::ios_base::app);
                outfile << "DEFINE " << library << " " << lib_path << std::endl;
            } else if (!lib_ptr->isValid()) {
                throw std::invalid_argument("Invalid library: " + library);
            }

            // open technology file
            tech_ptr = oa::oaTech::find(lib_ptr);
            if (tech_ptr == nullptr) {
                // opened tech not found, attempt to open
                if (!oa::oaTech::exists(lib_ptr)) {
                    throw std::runtime_error("Cannot find technology for library: " + library);
                } else {
                    tech_ptr = oa::oaTech::open(lib_ptr, 'r');
                    if (tech_ptr == nullptr) {
                        throw std::runtime_error("Cannot open technology for library: " + library);
                    }
                }
            }

            // get database unit
            dbu_per_uu = tech_ptr->getDBUPerUU(oa::oaViewType::get(oa::oacMaskLayout));

            is_open = true;
        } catch (oa::oaCompatibilityError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA Compatibility Error: " + msg_std);
        } catch (oa::oaDMError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA DM Error: " + msg_std);
        } catch (oa::oaError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA Error: " + msg_std);
        } catch (oa::oaDesignError &ex) {
            std::string msg_std(ex.getMsg());
            throw std::runtime_error("OA Design Error: " + msg_std);
        }
    }

    cbag::CSchInstance Library::parse_sch(const std::string &cell_name,
                                          const std::string &view_name = "schematic") {
        // get OA Block pointer
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name_oa, cell_oa, view_oa,
                                                   oa::oaViewType::get(oa::oacSchematic), 'r');
        if (dsn_ptr == nullptr) {
            std::ostringstream stream;
            stream << "Cannot open cell: " << lib_name << "__" << cell_name
                   << "(" << view_name << ").";
            throw std::invalid_argument(stream.str());
        }

        oa::oaBlock *blk_ptr = dsn_ptr->getTopBlock();
        if (blk_ptr == nullptr) {
            std::ostringstream stream;
            stream << "Cannot open top block for cell: " << lib_name << "__" << cell_name
                   << "(" << view_name << ").";
            throw std::invalid_argument(stream.str());

        }

        // place holder classes
        oa::oaString tmp_str;

        // get all terminals
        std::vector<cbag::CSchTerm> in_terms, out_terms, inout_terms;
        // get bus terminals
        oa::oaIter<oa::oaBusTermDef> bus_term_def_iter(blk_ptr->getBusTermDefs());
        oa::oaBusTermDef *bus_term_def_ptr;
        std::vector<cbag::CSchTerm> *term_list_ptr = &in_terms;
        while ((bus_term_def_ptr = bus_term_def_iter.getNext()) != nullptr) {
            bus_term_def_ptr->getName(ns_cdba, tmp_str);
            std::string pin_name(tmp_str);

            oa::oaIter<oa::oaBusTermBit> bus_term_iter(bus_term_def_ptr->getBusTermBits());
            oa::oaBusTermBit *bus_term_ptr;
            std::vector<uint32_t> idx_list;
            while ((bus_term_ptr = bus_term_iter.getNext()) != nullptr) {
                switch (bus_term_ptr->getTermType()) {
                    case oa::oacInputTermType:
                        term_list_ptr = &in_terms;
                        break;
                    case oa::oacOutputTermType:
                        term_list_ptr = &out_terms;
                        break;
                    default:
                        term_list_ptr = &inout_terms;
                        break;
                }
                idx_list.push_back(bus_term_ptr->getBitIndex());
            }
            term_list_ptr->emplace_back(pin_name, idx_list);
        }
        // get scalar terminals
        oa::oaIter<oa::oaTerm> term_iter(blk_ptr->getTerms(oacTermIterSingleBit));
        oa::oaTerm *term_ptr;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(ns_cdba, tmp_str);
            if (tmp_str.index('<') == tmp_str.getLength()) {
                std::string pin_name(tmp_str);
                switch (term_ptr->getTermType()) {
                    case oa::oacInputTermType:
                        term_list_ptr = &in_terms;
                        break;
                    case oa::oacOutputTermType:
                        term_list_ptr = &out_terms;
                        break;
                    default:
                        term_list_ptr = &inout_terms;
                        break;
                }
                term_list_ptr->emplace_back(pin_name);
            }
        }

        // get instances
        oa::oaIter<oa::oaInst> inst_iter(blk_ptr->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString inst_lib_oa, inst_cell_oa;
            inst_ptr->getLibName(ns_cdba, inst_lib_oa);
            inst_ptr->getCellName(ns_cdba, inst_cell_oa);
            // NOTE: exclude pin instances
            if (inst_lib_oa != "basic" ||
                (inst_cell_oa != "ipin" && inst_cell_oa != "opin" && inst_cell_oa != "iopin")) {
                // get names
                std::string inst_lib(inst_lib_oa);
                std::string inst_cell(inst_cell_oa);

                inst_ptr->getViewName(ns_cdba, tmp_str);
                std::string inst_view(tmp_str);

                inst_ptr->getName(ns_cdba, tmp_str);
                std::string inst_name(tmp_str);

                // get transform
                oa::oaTransform xform;
                inst_ptr->getTransform(xform);
                cbag::Transform inst_xform(xform.xOffset(), xform.yOffset(),
                                           convert_orient(xform.orient()));

                if (inst_ptr->hasProp()) {
                    oa::oaIter<oa::oaProp> prop_iter(inst_ptr->getProps());
                    oa::oaProp *prop_ptr = prop_iter.getNext();
                    prop_ptr->getName(tmp_str);
                    std::cout << ", {" << tmp_str << "(" << get_type(prop_ptr) << ")=";
                    prop_ptr->getValue(tmp_str);
                    std::cout << tmp_str;
                    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                        prop_ptr->getName(tmp_str);
                        std::cout << ", " << tmp_str << "(" << get_type(prop_ptr) << ")=";
                        prop_ptr->getValue(tmp_str);
                        std::cout << tmp_str;
                    }
                    std::cout << "}";
                }

                std::cout << ")" << std::endl;
            }
        }
    }

    void Library::close() {
        if (is_open) {
            tech_ptr->close();
            lib_ptr->close();

            is_open = false;
        }

    }

}