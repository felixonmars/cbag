
#include <iostream>

#include <oa/oaDesignDB.h>

template<class T>
void read_prop(T *dsn_ptr) {
    // read properties
    oa::oaIter<oa::oaProp> prop_iter(dsn_ptr->getProps());
    oa::oaProp *prop_ptr;
    oa::oaString tmp_str;
    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
        prop_ptr->getName(tmp_str);
        std::cout << "Property: " << tmp_str << std::endl;
        std::cout << "  has_prop: " << prop_ptr->hasProp() << std::endl;
        std::cout << "  has_AppDef: " << prop_ptr->hasAppDef() << std::endl;
        std::cout << "  type: " << prop_ptr->getType().getName() << std::endl;
        switch (prop_ptr->getType()) {
            case oa::oacStringPropType : {
                prop_ptr->getValue(tmp_str);
                std::cout << "  value: " << tmp_str << std::endl;
                break;
            }
            case oa::oacIntPropType : {
                std::cout << "  value: "
                          << static_cast<oa::oaIntProp *>(prop_ptr)->getValue() // NOLINT
                          << std::endl;
                break;
            }
            case oa::oacDoublePropType : {
                std::cout << "  value: "
                          << static_cast<oa::oaDoubleProp *>(prop_ptr)->getValue() // NOLINT
                          << std::endl;
                break;
            }
            case oa::oacTimePropType : {
                std::cout << "  value: "
                          << static_cast<oa::oaTimeProp *>(prop_ptr)->getValue() // NOLINT
                          << std::endl;
                break;
            }
            case oa::oacAppPropType : {
                oa::oaByteArray data;
                oa::oaString app_str;
                oa::oaAppProp *app_ptr = static_cast<oa::oaAppProp *>(prop_ptr); // NOLINT
                app_ptr->getValue(data); // NOLINT
                app_ptr->getAppType(app_str);
                std::cout << "  value: " << std::string(data.getElements(),
                                                        data.getElements() + data.getNumElements())
                          << std::endl;
                break;
            }
            default : {
                std::cout << "value: unsupported type" << std::endl;
            }
        }
    }

    oa::oaIter<oa::oaAppDef> appdef_iter(dsn_ptr->getAppDefs());
    oa::oaAppDef *appdef_ptr;
    while ((appdef_ptr = appdef_iter.getNext()) != nullptr) {
        oa::oaString app_str;
        appdef_ptr->getName(app_str);
        oa::oaType app_type = appdef_ptr->getType();
        std::cout << "AppDef " << app_str << std::endl;
        std::cout << "  has_prop: " << appdef_ptr->hasProp() << std::endl;
        std::cout << "  has_AppDef: " << appdef_ptr->hasAppDef() << std::endl;
        std::cout << "  type: " << app_type.getName() << std::endl;
        switch (app_type) {
            case oa::oacIntAppDefType :
                std::cout << "  value: "
                          << (static_cast<oa::oaIntAppDef<T> *>(appdef_ptr))->get( // NOLINT
                                  dsn_ptr) << std::endl;
                break;
            case oa::oacStringAppDefType :
                (static_cast<oa::oaStringAppDef<T> *>(appdef_ptr))->get(dsn_ptr,
                                                                                   app_str);
                std::cout << "  value: " << app_str << std::endl;
                break;
            default:
                break;
        }
    }

}


int main(int argc, char *argv[]) {
    try {
        std::string cell_name;
        if (argc < 2) {
            cell_name = "inv";
        } else {
            cell_name = std::string(argv[1]);
        }

        const oa::oaCdbaNS ns;

        std::string lib_file("cds.lib");
        std::string lib_name("schtest");
        std::string view_name("schematic");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "NotAssignable"
        oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, // NOLINT
                     oacDataModelRevNumber); // NOLINT
#pragma clang diagnostic pop

        oa::oaLibDefList::openLibs(lib_file.c_str());

        std::cout << "Opened libraries:" << std::endl;
        oa::oaIter<oa::oaLib> lib_iter(oa::oaLib::getOpenLibs());
        oa::oaLib *lib_ptr;
        while ((lib_ptr = lib_iter.getNext()) != nullptr) {
            oa::oaString tmp_str;
            lib_ptr->getName(ns, tmp_str);
            std::cout << tmp_str << std::endl;
        }

        oa::oaScalarName lib_oa(ns, lib_name.c_str());
        oa::oaScalarName cell_oa(ns, cell_name.c_str());
        oa::oaScalarName view_oa(ns, view_name.c_str());

        oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, 'r');

        std::cout << "Design:" << std::endl;
        std::cout << "create time: " << dsn_ptr->getCreateTime() << std::endl;
        std::cout << "Last save time: " << dsn_ptr->getLastSavedTime() << std::endl;
        std::cout << "Cp ID: " << dsn_ptr->getCpID() << std::endl;
        std::cout << "undo enabled: " << dsn_ptr->isUndoEnabled() << std::endl;
        std::cout << "num undo cp recs: " << dsn_ptr->getNumUndoCpRecs() << std::endl;
        std::cout << "timestamp enabled: " << dsn_ptr->areTimeStampsEnabled() << std::endl;

        for (unsigned int i = 0; i <= 80; i++) {
            auto e = static_cast<oa::oaDesignDataTypeEnum >(i);
            std::cout << "Type: " << oa::oaDesignDataType(e).getName() << std::endl;
            try {
                std::cout << "timestamp: " << dsn_ptr->getTimeStamp(e) << std::endl;
            } catch (...) {
                std::cout << "timestamp: none" << std::endl;
            }
        }

        read_prop(dsn_ptr);

        oa::oaIter<oa::oaAppObjectDef> objdef_iter(dsn_ptr->getAppObjectDefs());
        oa::oaAppObjectDef *objdef_ptr;
        while ((objdef_ptr = objdef_iter.getNext()) != nullptr) {
            oa::oaString tmp_str;
            objdef_ptr->getName(tmp_str);
            oa::oaType objdef_type = objdef_ptr->getType();
            std::cout << "AppObjDef " << tmp_str << std::endl;
            std::cout << "  has_prop: " << objdef_ptr->hasProp() << std::endl;
            std::cout << "  has_AppDef: " << objdef_ptr->hasAppDef() << std::endl;
            std::cout << "  type: " << objdef_type.getName() << std::endl;
            std::cout << "  timestamp: " << objdef_ptr->getTimeStamp(dsn_ptr) << std::endl;
        }

        std::cout << "Block:" << std::endl;
        read_prop(dsn_ptr->getTopBlock());

        std::cout << "Module:" << std::endl;
        read_prop(dsn_ptr->getTopModule());

        dsn_ptr->close();

        std::cout << "Closing libraries" << std::endl;
        lib_iter = oa::oaIter<oa::oaLib>(oa::oaLib::getOpenLibs());
        while ((lib_ptr = lib_iter.getNext()) != nullptr) {
            oa::oaString tmp_str;
            lib_ptr->getName(ns, tmp_str);
            std::cout << "Closing library: " << tmp_str << std::endl;
            lib_ptr->close();
        }
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
        throw std::runtime_error("OA Design Error: " + std::string(ex.getMsg()));
    }
}
