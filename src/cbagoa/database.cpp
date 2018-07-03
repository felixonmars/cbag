#include <cbagoa/database.h>
#include <iostream>

namespace cbagoa {

    cbag::Orientation convert_orient(const oa::oaOrient &orient) {
        switch (orient) {
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

    Library::~Library() {
        close();
    }

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

    cbag::CSchMaster Library::parse_sch(const std::string &cell_name,
                                        const std::string &view_name) {
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

        cbag::CSchMaster ans;
        // place holder classes
        oa::oaString tmp_str;

        // get bus terminals
        oa::oaIter<oa::oaBusTermDef> bus_term_def_iter(blk_ptr->getBusTermDefs());
        oa::oaBusTermDef *bus_term_def_ptr;
        std::list<cbag::CSchTerm> *term_list_ptr = nullptr;
        while ((bus_term_def_ptr = bus_term_def_iter.getNext()) != nullptr) {
            bus_term_def_ptr->getName(ns_cdba, tmp_str);
            std::string pin_name(tmp_str);

            oa::oaIter<oa::oaBusTermBit> bus_term_iter(bus_term_def_ptr->getBusTermBits());
            oa::oaBusTermBit *bus_term_ptr;
            std::list<uint32_t> idx_list;
            term_list_ptr = nullptr;
            while ((bus_term_ptr = bus_term_iter.getNext()) != nullptr) {
                if (term_list_ptr == nullptr) {
                    switch (bus_term_ptr->getTermType()) {
                        case oa::oacInputTermType:
                            term_list_ptr = &ans.in_terms;
                            break;
                        case oa::oacOutputTermType:
                            term_list_ptr = &ans.out_terms;
                            break;
                        default:
                            term_list_ptr = &ans.io_terms;
                    }
                }
                idx_list.push_back(bus_term_ptr->getBitIndex());
            }
            if (term_list_ptr != nullptr) {
                term_list_ptr->emplace_back(pin_name, idx_list);
            }
        }
        // get scalar terminals
        oa::oaIter<oa::oaTerm> term_iter(blk_ptr->getTerms(oacTermIterSingleBit));
        oa::oaTerm *term_ptr;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(ns_cdba, tmp_str);
            // avoid single-bit bus terminals, i.e. bus with single index
            if (tmp_str.index('<') == tmp_str.getLength()) {
                std::string pin_name(tmp_str);
                switch (term_ptr->getTermType()) {
                    case oa::oacInputTermType:
                        ans.in_terms.emplace_back(pin_name);
                        break;
                    case oa::oacOutputTermType:
                        ans.out_terms.emplace_back(pin_name);
                        break;
                    default:
                        ans.io_terms.emplace_back(pin_name);
                }
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
                cbag::Range inst_range;
                oa::oaType inst_type = inst_ptr->getType();
                if (inst_type == oa::oacVectorInstType) {
                    auto vinst_ptr = static_cast<oa::oaVectorInst *>(inst_ptr); // NOLINT
                    int32_t start = vinst_ptr->getStart();
                    int32_t stop = vinst_ptr->getStop();
                    int32_t step = (stop > start) ? 1 : -1;
                    inst_range = cbag::Range(start, stop, step);
                } else if (inst_type == oa::oacVectorInstBitType) {
                    auto vinst_ptr = static_cast<oa::oaVectorInstBit *>(inst_ptr); // NOLINT
                    int32_t start = vinst_ptr->getBitIndex();
                    inst_range = cbag::Range(start, start - 1, -1);
                }

                inst_ptr->getViewName(ns_cdba, tmp_str);
                std::string inst_view(tmp_str);

                inst_ptr->getName(ns_cdba, tmp_str);
                std::string inst_name(tmp_str);

                // get transform
                oa::oaTransform xform;
                inst_ptr->getTransform(xform);
                cbag::Transform inst_xform(xform.xOffset(), xform.yOffset(),
                                           convert_orient(xform.orient()));

                // create schematic instance
                ans.inst_list.emplace_back(inst_name, inst_lib, inst_cell, inst_view,
                                           inst_xform, inst_range);
                cbag::CSchInstance *sch_inst_ptr = &ans.inst_list.back();

                // get parameters
                if (inst_ptr->hasProp()) {
                    oa::oaIter<oa::oaProp> prop_iter(inst_ptr->getProps());
                    oa::oaProp *prop_ptr;
                    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                        add_param(sch_inst_ptr->params, prop_ptr);
                    }
                }

                // get instance terminal connections
                uint32_t inst_size = sch_inst_ptr->size();
                oa::oaIter<oa::oaInstTerm> iterm_iter(inst_ptr->getInstTerms(oacInstTermIterAll));
                oa::oaInstTerm *iterm_ptr;
                while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
                    iterm_ptr->getTermName(ns_cdba, tmp_str);
                    auto iter_flag_pair = sch_inst_ptr->term_map.emplace(tmp_str, inst_size);
                    auto pair_iter = iter_flag_pair.first;

                    std::cout << "  Terminal: " << tmp_str << std::endl;
                    std::cout << "    numBits: " << iterm_ptr->getNumBits() << std::endl;
                    std::cout << "    isBound: " << iterm_ptr->isBound() << std::endl;
                    std::cout << "    isImplicit: " << iterm_ptr->isImplicit() << std::endl;
                    std::cout << "    usesTermPosition: " << iterm_ptr->usesTermPosition() << std::endl;
                    iterm_ptr->getNet()->getName(ns_cdba, tmp_str);
                    std::cout << "    Net: " << tmp_str << std::endl;
                }

            }
        }

        dsn_ptr->close();
        return ans;
    }

    void Library::close() {
        if (is_open) {
            tech_ptr->close();
            lib_ptr->close();

            is_open = false;
        }

    }

    void add_param(cbag::ParamMap &params, oa::oaProp *prop_ptr) {
        // get parameter name
        oa::oaString tmp_str;
        prop_ptr->getName(tmp_str);
        std::string key(tmp_str);

        // get parameter value
        // NOTE: static_cast for down-casting is bad, but openaccess API sucks...
        // use NOLINT to suppress IDE warnings
        oa::oaType ptype = prop_ptr->getType();
        switch (ptype) {
            case oa::oacStringPropType : {
                prop_ptr->getValue(tmp_str);
                std::string vals(tmp_str);
                params.emplace(key, vals);
                break;
            }
            case oa::oacIntPropType : {
                params.emplace(key, static_cast<oa::oaIntProp *>(prop_ptr)->getValue()); // NOLINT
                break;
            }
            case oa::oacDoublePropType : {
                params.emplace(key,
                               static_cast<oa::oaDoubleProp *>(prop_ptr)->getValue()); // NOLINT
                break;
            }
            case oa::oacFloatPropType : {
                double vald = static_cast<oa::oaFloatProp *>(prop_ptr)->getValue(); // NOLINT
                params.emplace(key, vald);
                break;
            }
            default : {
                std::ostringstream stream;
                stream << "Unsupport OA property type: " << ptype.getName() << ".  See developer.";
                throw std::invalid_argument(stream.str());
            }
        }
    }

}