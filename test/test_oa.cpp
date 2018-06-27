#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <stdexcept>
#include <map>
#include <vector>

#include <oa/oaDesignDB.h>

int read_oa() {
    try {

        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

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

        // print bus terminals
        oa::oaIter<oa::oaBusTermDef> bus_term_def_iter(blk_ptr->getBusTermDefs());
        oa::oaBusTermDef *bus_term_def_ptr;
        while ((bus_term_def_ptr = bus_term_def_iter.getNext()) != nullptr) {
            bus_term_def_ptr->getName(ns_cdba, tmp_str);
            std::cout << "Bus Term: " << tmp_str << std::endl;
            oa::oaIter<oa::oaBusTermBit> bus_term_iter(bus_term_def_ptr->getBusTermBits());
            oa::oaBusTermBit *bus_term_ptr;
            bool print_type = false;
            while ((bus_term_ptr = bus_term_iter.getNext()) != nullptr) {
                if (!print_type) {
                    std::cout << "type: " << bus_term_ptr->getTermType().getName() << std::endl;
                    print_type = true;
                }
                bus_term_ptr->getName(tmp_vec_name);
                tmp_vec_name.getBaseName(ns_cdba, tmp_str);
                std::cout << "base = " << tmp_str << ", idx = " << tmp_vec_name.getIndex() << std::endl;
            }
        }

        // print terminals
        oa::oaIter<oa::oaTerm> term_iter(blk_ptr->getTerms(oacTermIterSingleBit));
        oa::oaTerm *term_ptr;
        while ((term_ptr = term_iter.getNext()) != nullptr) {
            term_ptr->getName(tmp_name);
            tmp_name.get(ns_cdba, tmp_str);
            if (tmp_str.index('<') == tmp_str.getLength()) {
                std::cout << "Term(" << tmp_str << ", " << term_ptr->getTermType().getName() << ")" << std::endl;
            }
        }

        // print instances
        oa::oaIter<oa::oaInst> inst_iter(blk_ptr->getInsts());
        oa::oaInst *inst_ptr;
        while ((inst_ptr = inst_iter.getNext()) != nullptr) {
            oa::oaString lib_str, cell_str;
            inst_ptr->getLibName(ns_cdba, lib_str);
            inst_ptr->getCellName(ns_cdba, cell_str);
            if (lib_str != "basic" || (cell_str != "ipin" && cell_str != "opin" && cell_str != "iopin")) {
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
                    std::cout << ", {" << tmp_str << "=";
                    prop_ptr->getValue(tmp_str);
                    std::cout << tmp_str;
                    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
                        prop_ptr->getName(tmp_str);
                        std::cout << ", " << tmp_str << "=";
                        prop_ptr->getValue(tmp_str);
                        std::cout << tmp_str;
                    }
                    std::cout << "}";
                }

                std::cout << ")" << std::endl;
            }
        }


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

int write_oa() {
    try {

        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

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


int main(int argc, char *argv[]) {
    return read_oa();
    // return write_oa(argc, argv);
}
