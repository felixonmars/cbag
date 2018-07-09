#include <iostream>

#include <cbag/spirit/parsers.h>
#include <cbag/spirit/name.h>
#include <cbag/spirit/name_unit.h>

#include <cbagoa/database.h>


namespace cbagoa {

    namespace bsp = cbag::spirit;

    bsp::ast::name parse_name(const std::string &source) {
        return cbag::parse<bsp::ast::name, bsp::parser::name_type>(source, bsp::name());
    }

    bsp::ast::name_unit parse_name_unit(const std::string &source) {
        return cbag::parse<bsp::ast::name_unit,
                bsp::parser::name_unit_type>(source, bsp::name_unit());
    }

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
        throw std::runtime_error("OA Error: " + std::string(msg));
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
            throw std::runtime_error("OA Compatibility Error: " + std::string(ex.getMsg()));
        } catch (oa::oaDMError &ex) {
            throw std::runtime_error("OA DM Error: " + std::string(ex.getMsg()));
        } catch (oa::oaError &ex) {
            throw std::runtime_error("OA Error: " + std::string(ex.getMsg()));
        } catch (oa::oaDesignError &ex) {
            throw std::runtime_error("OA Design Error: " + std::string(ex.getMsg()));
        }
    }

    cbag::CSchMaster Library::parse_sch(const std::string &cell_name,
                                        const std::string &view_name) {
        // get OA Block pointer
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name_oa, cell_oa, view_oa,
                                                   oa::oaViewType::get(oa::oacSchematic), 'r');
        std::ostringstream errstream;
        if (dsn_ptr == nullptr) {
            errstream << "Cannot open cell: " << lib_name << "__" << cell_name
                      << "(" << view_name << ").";
            throw std::invalid_argument(errstream.str());
        }

        oa::oaBlock *blk_ptr = dsn_ptr->getTopBlock();
        if (blk_ptr == nullptr) {
            errstream << "Cannot open top block for cell: " << lib_name << "__" << cell_name
                      << "(" << view_name << ").";
            throw std::invalid_argument(errstream.str());

        }

        // place holder classes
        oa::oaString tmp_str;
        // create schematic master
        cbag::CSchMaster ans;

        // get pins
        oa::oaIter<oa::oaPin> pin_iter(blk_ptr->getPins());
        oa::oaPin *pin_ptr;
        while ((pin_ptr = pin_iter.getNext()) != nullptr) {
            oa::oaTerm *term_ptr = pin_ptr->getTerm();
            oa::oaString tmp_;
            term_ptr->getName(ns_cdba, tmp_str);
            bool success;
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    success = ans.in_pins.insert(parse_name(std::string(tmp_str))).second;
                    break;
                case oa::oacOutputTermType :
                    success = ans.out_pins.insert(parse_name(std::string(tmp_str))).second;
                    break;
                case oa::oacInputOutputTermType :
                    success = ans.io_pins.insert(parse_name(std::string(tmp_str))).second;
                    break;
                default:
                    term_ptr->getName(ns_cdba, tmp_str);
                    errstream << "Pin " << tmp_str << " has invalid terminal type: "
                              << term_ptr->getTermType().getName();
                    throw std::invalid_argument(errstream.str());
            }
            if (!success) {
                errstream << "Cannot add pin " << tmp_str << "; it already exists.";
                throw std::invalid_argument(errstream.str());
            }
        }

        // get instances
        oa::oaIter<oa::oaInst> inst_iter(blk_ptr->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString inst_lib_oa, inst_cell_oa, inst_name_oa;
            inst_ptr->getLibName(ns_cdba, inst_lib_oa);
            inst_ptr->getCellName(ns_cdba, inst_cell_oa);
            // NOTE: exclude pin instances
            if (inst_lib_oa != "basic" ||
                (inst_cell_oa != "ipin" && inst_cell_oa != "opin" && inst_cell_oa != "iopin")) {
                // get view
                oa::oaString inst_view_oa;
                inst_ptr->getViewName(ns_cdba, inst_view_oa);

                // get instance name
                inst_ptr->getName(ns_cdba, inst_name_oa);

                // get transform
                oa::oaTransform xform;
                inst_ptr->getTransform(xform);

                // create schematic instance
                bsp::ast::name_unit inst_name = parse_name_unit(std::string(inst_name_oa));
                if (inst_name.mult > 1) {
                    errstream << "Invalid instance name: " << inst_name_oa;
                    throw std::invalid_argument(errstream.str());
                }
                uint32_t inst_size = inst_name.size();

                auto inst_ret_val = ans.inst_map.emplace(std::move(inst_name),
                                                         cbag::CSchInstance(std::string(inst_lib_oa),
                                                                            std::string(inst_cell_oa),
                                                                            std::string(inst_view_oa),
                                                                            cbag::Transform(xform.xOffset(),
                                                                                            xform.yOffset(),
                                                                                            convert_orient(
                                                                                                    xform.orient()))));
                if (!inst_ret_val.second) {
                    errstream << "Instance " << inst_name_oa << " already exists.";
                    throw std::invalid_argument(errstream.str());
                }

                // get instance pointer
                cbag::CSchInstance *sinst_ptr = &inst_ret_val.first->second;

                // get parameters
                if (inst_ptr->hasProp()) {
                    oa::oaIter<oa::oaProp> prop_iter(inst_ptr->getProps());
                    oa::oaProp *prop_ptr;
                    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                        add_param(sinst_ptr->params, prop_ptr);
                    }
                }

                // get instance connections
                oa::oaIter<oa::oaInstTerm> iterm_iter(inst_ptr->getInstTerms(oacInstTermIterAll));
                oa::oaInstTerm *iterm_ptr;
                while ((iterm_ptr = iterm_iter.getNext()) != nullptr) {
                    // register instance terminals in connection map
                    iterm_ptr->getTerm()->getName(ns_cdba, tmp_str);
                    bsp::ast::name term_name = parse_name(std::string(tmp_str));

                    iterm_ptr->getNet()->getName(ns_cdba, tmp_str);
                    bsp::ast::name net_name = parse_name(std::string(tmp_str));

                    // populate connect map keys

                    // populate connection maps
                    if (inst_size == 1) {

                    }
                }
            }
        }

        // close design and return master
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