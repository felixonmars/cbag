
#include <iostream>

#include <oa/oaDesignDB.h>


int main(int argc, char *argv[]) {

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

    oa::oaScalarName lib_oa(ns, lib_name.c_str());
    oa::oaScalarName cell_oa(ns, cell_name.c_str());
    oa::oaScalarName view_oa(ns, view_name.c_str());

    oa::oaDesign *dsn_ptr = oa::oaDesign::open(lib_oa, cell_oa, view_oa, 'r');

    // read properties
    oa::oaIter<oa::oaProp> prop_iter(dsn_ptr->getProps());
    oa::oaProp *prop_ptr;
    oa::oaString tmp_str;
    while ((prop_ptr = prop_iter.getNext()) != nullptr) {
        prop_ptr->getName(tmp_str);
        std::cout << "Property: " << tmp_str << std::endl;
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
        std::cout << "  type: " << app_type.getName() << std::endl;
        switch (app_type) {
            case oa::oacIntAppDefType :
                std::cout << "  value: "
                          << (static_cast<oa::oaIntAppDef<oa::oaDesign> *>(appdef_ptr))->get( // NOLINT
                                  dsn_ptr) << std::endl;
                break;
            case oa::oacStringAppDefType :
                (static_cast<oa::oaStringAppDef<oa::oaDesign> *>(appdef_ptr))->get(dsn_ptr,
                                                                                   app_str);
                std::cout << "  value: " << app_str << std::endl;
                break;
            default:
                break;
        }
    }

    dsn_ptr->close();
}
