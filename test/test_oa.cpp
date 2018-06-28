#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>

#include <yaml-cpp/yaml.h>
#include <oa/oaDesignDB.h>

#include <cbag/schematic/objects.h>


std::string get_type(oa::oaProp *prop_ptr) {
    switch (prop_ptr->getType()) {
        case oa::oacStringPropType :
            return "str";
        case oa::oacIntPropType :
            return "int";
        case oa::oacDoublePropType :
            return "dbl";
        case oa::oacFloatPropType :
            return "flt";
        default :
            throw std::invalid_argument("Unsupport OA property type.  See developer.");
    }
}

int read_oa() {
    try {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        const oa::oaNativeNS ns;
        const oa::oaCdbaNS ns_cdba;

        oa::oaString lib_def_path("cds.lib");
        oa::oaLibDefList::openLibs(lib_def_path);

        // open library
        oa::oaScalarName lib_name(ns, "AAAAAASCRATCH");
        oa::oaScalarName cell_name(ns, "inv");
        oa::oaScalarName view_name(ns, "schematic");
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name);
        if (lib_ptr == nullptr) {
            throw std::invalid_argument("Cannot find library.");
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library.");
        }

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name, cell_name, view_name,
                                                   oa::oaViewType::get(oa::oacSchematic), 'r');
        oa::oaBlock *blk_ptr = dsn_ptr->getTopBlock();


        oa::oaName tmp_name;
        oa::oaVectorBitName tmp_vec_name;
        oa::oaString tmp_str;

        YAML::Emitter out_yaml;
        out_yaml << YAML::BeginMap;


        // get all terminals
        std::vector<cbag::CSchTerm> in_terms, out_terms, inout_terms;
        // get bus terminals
        oa::oaIter<oa::oaBusTermDef> bus_term_def_iter(blk_ptr->getBusTermDefs());
        oa::oaBusTermDef *bus_term_def_ptr;
        std::vector<cbag::CSchTerm> *term_list_ptr = nullptr;
        while ((bus_term_def_ptr = bus_term_def_iter.getNext()) != nullptr) {
            bus_term_def_ptr->getName(ns_cdba, tmp_str);
            std::string pin_name(tmp_str);

            oa::oaIter<oa::oaBusTermBit> bus_term_iter(bus_term_def_ptr->getBusTermBits());
            oa::oaBusTermBit *bus_term_ptr;
            std::vector<uint32_t> idx_list;
            while ((bus_term_ptr = bus_term_iter.getNext()) != nullptr) {
                if (term_list_ptr == nullptr) {
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
                }
                bus_term_ptr->getName(tmp_vec_name);
                idx_list.push_back(tmp_vec_name.getIndex());
            }
            if (term_list_ptr != nullptr) {
                term_list_ptr->emplace_back(pin_name, idx_list);
            }
        }

        // get scalar terminals
        oa::oaIter<oa::oaTerm> term_iter(blk_ptr->getTerms(oacTermIterSingleBit));
        oa::oaTerm *term_ptr;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(tmp_name);
            tmp_name.get(ns_cdba, tmp_str);
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

        // write to YAML
        out_yaml << YAML::Key << "pins"
                 << YAML::Value
                 << YAML::BeginMap
                 << YAML::Key << "inputs"
                 << YAML::Value << in_terms
                 << YAML::Key << "outputs"
                 << YAML::Value << out_terms
                 << YAML::Key << "inouts"
                 << YAML::Value << inout_terms
                 << YAML::EndMap;

        // print instances
        oa::oaIter<oa::oaInst> inst_iter(blk_ptr->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString lib_str, cell_str;
            inst_ptr->getLibName(ns_cdba, lib_str);
            inst_ptr->getCellName(ns_cdba, cell_str);
            if (lib_str != "basic" ||
                (cell_str != "ipin" && cell_str != "opin" && cell_str != "iopin")) {
                inst_ptr->getName(ns_cdba, tmp_str);
                std::cout << "Inst(" << tmp_str << ", " << lib_str << ", " << cell_str;
                inst_ptr->getViewName(ns_cdba, tmp_str);
                std::cout << ", " << tmp_str;

                oa::oaTransform xform;
                inst_ptr->getTransform(xform);
                std::cout << ", (" << xform.xOffset() << ", " << xform.yOffset() << ")";
                std::cout << ", " << xform.orient().getName();

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

        out_yaml << YAML::EndMap;
        std::cout << out_yaml.c_str() << std::endl;

        lib_ptr->close();
    } catch (oa::oaCompatibilityError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Compatibility Error: " + msg_std);
    } catch (oa::oaDMError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA DM Error: " + msg_std);
    } catch (oa::oaError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Error: " + msg_std);
    }

    return 0;
}

/*
int write_oa() {
    try {

#pragma clang diagnostic push
#pragma ide diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        const oa::oaNativeNS ns;
        oa::oaString lib_def_path("cds.lib");
        oa::oaLibDefList::openLibs(lib_def_path);

        // open library
        oa::oaScalarName lib_name(ns, "AAAscratch");
        oa::oaScalarName cell_name(ns, "oa_mos_write");
        oa::oaScalarName view_name(ns, "layout");
        oa::oaLib *lib_ptr = oa::oaLib::find(lib_name);
        if (lib_ptr == nullptr) {
            throw std::invalid_argument("Cannot find library.");
        } else if (!lib_ptr->isValid()) {
            throw std::invalid_argument("Invalid library.");
        }

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_name, cell_name, view_name,
                                                   oa::oaViewType::get(oa::oacMaskLayout), 'w');


        oa::oaBlock *blk_ptr = oa::oaBlock::create(dsn_ptr);

        oa::oaScalarName mlib_name(ns, "tsmcN16");
        oa::oaScalarName mcell_name(ns, "nch_lvt_mac");
        oa::oaDesign *master_ptr = oa::oaDesign::open(mlib_name, mcell_name, view_name,
                                                      oa::oaViewType::get(oa::oacMaskLayout), 'r');


        oa::oaScalarName inst_name(ns, "X0");
        oa::oaTransform inst_tran(oa::oaPoint(0, 0));
        oa::oaParamArray parr;

        std::ifstream myfile("oa_mos_params.txt");
        std::string line;
        while (std::getline(myfile, line)) {
            std::istringstream iss(line);
            std::string name;
            std::string value;
            iss >> name;
            iss >> value;
            oa::oaParam par;
            par.setName(name.c_str());
            par.setStringVal(value.c_str());
            parr.append(par);
            std::cout << "adding par " << name << ": " << value << std::endl;
        }


        oa::oaScalarInst::create(blk_ptr, master_ptr, inst_name, inst_tran, &parr);

        dsn_ptr->save();
        dsn_ptr->close();
        lib_ptr->close();
    } catch (oa::oaCompatibilityError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Compatibility Error: " + msg_std);
    } catch (oa::oaDMError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA DM Error: " + msg_std);
    } catch (oa::oaError &ex) {
        std::string msg_std(ex.getMsg());
        throw std::runtime_error("OA Error: " + msg_std);
    }

    return 0;
}
*/

int main(int argc, char *argv[]) {
    return read_oa();
    // return write_oa(argc, argv);
}
