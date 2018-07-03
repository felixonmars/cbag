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

        // get pins
        oa::oaCollection<oa::oaPin, oa::oaBlock> pin_list = blk_ptr->getPins();
        // count number of pins for each type
        unsigned long num_in = 0;
        unsigned long num_out = 0;
        unsigned long num_io = 0;
        oa::oaIter<oa::oaPin> pin_iter(pin_list);
        oa::oaPin *pin_ptr;
        while ((pin_ptr = pin_iter.getNext()) != nullptr) {
            switch (pin_ptr->getTerm()->getTermType()) {
                case oa::oacInputTermType :
                    num_in++;
                    break;
                case oa::oacOutputTermType :
                    num_out++;
                    break;
                case oa::oacInputOutputTermType :
                    num_io++;
                    break;
                default:
                    pin_ptr->getName(tmp_str);
                    errstream << "Pin " << tmp_str
                              << " has invalid terminal type: " << pin_ptr->getTerm()->getTermType().getName();
                    throw std::invalid_argument(errstream.str());
            }
        }

        // get number of instances.  Excluding pin instances
        oa::oaCollection<oa::oaInst, oa::oaBlock> inst_list = blk_ptr->getInsts();
        oa::oaIter<oa::oaInst> inst_iter(inst_list);
        oa::oaInst *inst_ptr;
        unsigned long num_inst = 0;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString inst_lib_oa, inst_cell_oa;
            inst_ptr->getLibName(ns_cdba, inst_lib_oa);
            inst_ptr->getCellName(ns_cdba, inst_cell_oa);
            if (inst_lib_oa != "basic" ||
                (inst_cell_oa != "ipin" && inst_cell_oa != "opin" && inst_cell_oa != "iopin")) {
                num_inst++;
            }
        }

        // create schematic master
        cbag::CSchMaster ans(num_in, num_out, num_io, num_inst);

        // populate pins
        unsigned long idx_in = 0;
        unsigned long idx_out = 0;
        unsigned long idx_io = 0;
        pin_iter = oa::oaIter<oa::oaPin>(pin_list);
        while ((pin_ptr = pin_iter.getNext()) != nullptr) {
            oa::oaTerm *term_ptr = pin_ptr->getTerm();
            switch (term_ptr->getTermType()) {
                case oa::oacInputTermType :
                    ans.in_pins[idx_in++] = make_name(term_ptr);
                    break;
                case oa::oacOutputTermType :
                    ans.out_pins[idx_out++] = make_name(term_ptr);
                    break;
                default:
                    ans.io_pins[idx_io++] = make_name(term_ptr);
            }
        }

        /*
        // get instances
        inst_iter = oa::oaIter<oa::oaInst>(inst_list);
        unsigned long idx_inst = 0;
        while((inst_ptr = inst_iter.getNext()) != nullptr) {
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

                // increment instance index
                idx_inst++;
            }
        }
        */
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

    cbag::Name Library::make_name(oa::oaTerm *term_ptr) {
        oa::oaString tmp_str;
        term_ptr->getName(ns_cdba, tmp_str);
        std::string term_str(tmp_str);
        return formatter.get_name(term_str);
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