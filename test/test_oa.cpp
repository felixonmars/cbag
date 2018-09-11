#include <fstream>

#include <oa/oaDesignDB.h>

#include <cbag/cbag.h>
#include <cbagoa/cbagoa.h>

void read_oa() {
    cbag::init_logging();

    std::string lib_file("cds.lib");
    std::string lib_path(".");
    std::string tech_lib("tsmcN16");
    const char *lib_name = "AAAFOO_TEST_STRONGARM";
    const char *cell_name = "inv";
    const char *cell_name2 = "inv2";
    const char *view_name = "schematic";
    const char *sym_view_name = "symbol";

    cbagoa::oa_database db(lib_file);

    cbag::sch::cellview sch_master = db.read_sch_cellview(lib_name, cell_name, view_name);
    db.write_sch_cellview(lib_name, cell_name2, view_name, true, sch_master);
    cbag::sch::cellview sym_master = db.read_sch_cellview(lib_name, cell_name, sym_view_name);
    db.write_sch_cellview(lib_name, cell_name2, sym_view_name, false, sym_master);

    db.read_sch_cellview(lib_name, cell_name2, view_name);
    db.read_sch_cellview(lib_name, cell_name2, sym_view_name);
}

void test_oa_box() {

    oa::oaBox box(0, 0, 5, 7);
    oa::oaPoint p;
    box.getCenter(p);
    std::cout << "center x: " << p.x() << std::endl;
    std::cout << "center y: " << p.y() << std::endl;
}

void print_tech_info() {
    oa::oaNativeNS ns;
    oa::oaString lib_def("cds.lib");
    oa::oaScalarName lib_name(ns, "tsmcN16");
    cbagoa::LibDefObserver obs(1);

    std::cout << "before init" << std::endl;
    oaDesignInit(oacAPIMajorRevNumber, oacAPIMinorRevNumber, oacDataModelRevNumber);

    std::cout << "open libs" << std::endl;
    oa::oaLibDefList::openLibs(lib_def);

    std::cout << "open tech" << std::endl;
    oa::oaTech *tech_ptr = oa::oaTech::find(lib_name);
    if (tech_ptr == nullptr) {
        tech_ptr = oa::oaTech::open(lib_name, 'r');
    }
    std::cout << "iteration" << std::endl;

    oa::oaString temp;
    oa::oaIter<oa::oaLayer> layers(tech_ptr->getLayers());
    std::cout << "layers:" << std::endl;
    while (oa::oaLayer *layer = layers.getNext()) {
        layer->getName(temp);
        oa::oaLayerNum id = layer->getNumber();
        std::cout << temp << ": " << id << std::endl;
    }
    oa::oaIter<oa::oaPurpose> purposes(tech_ptr->getPurposes());
    std::cout << "purposes:" << std::endl;
    while (oa::oaPurpose *purp = purposes.getNext()) {
        purp->getName(temp);
        oa::oaPurposeNum id = purp->getNumber();
        std::cout << temp << ": " << id << std::endl;
    }

    std::cout << "app obj defs:" << std::endl;
    oa::oaIter<oa::oaAppObjectDef> appdefs(tech_ptr->getAppObjectDefs());
    while (oa::oaAppObjectDef *appdef = appdefs.getNext()) {
        appdef->getName(temp);
        std::cout << temp << std::endl;
    }

    std::cout << "has prop: " << tech_ptr->hasProp() << std::endl;
    std::cout << "has app def: " << tech_ptr->hasAppDef() << std::endl;

    oa::oaAppObjectDef *obj_def = oa::oaAppObjectDef::find(oa::oaString("techLPP"));

    std::cout << "has app def: " << obj_def->hasAppDef() << std::endl;

    oa::oaIter<oa::oaAppObject> obj_iter(tech_ptr->getAppObjects(obj_def));
    auto *lay_def = oa::oaInterPointerAppDef<oa::oaAppObject>::find("lppLayer", obj_def);
    auto *layn_def = oa::oaIntAppDef<oa::oaAppObject>::find("lppLayerNum", obj_def);
    auto *pur_def = oa::oaInterPointerAppDef<oa::oaAppObject>::find("lppPurpose", obj_def);
    auto *purn_def = oa::oaIntAppDef<oa::oaAppObject>::find("lppPurposeNum", obj_def);

    while (oa::oaAppObject *obj = obj_iter.getNext()) {
        oa::oaObject *lay = lay_def->get(obj);
        oa::oaInt4 lay_n = layn_def->get(obj);
        oa::oaObject *pur = pur_def->get(obj);
        oa::oaInt4 pur_n = purn_def->get(obj);
        if (lay != nullptr) {
            auto *ptr = reinterpret_cast<oa::oaPhysicalLayer *>(lay);
            ptr->getName(temp);
            std::cout << "lay: " << temp << ": " << lay_n << std::endl;
        }
        if (pur != nullptr) {
            auto *ptr = reinterpret_cast<oa::oaPurpose *>(pur);
            ptr->getName(temp);
            std::cout << "purp: " << temp << ": " << pur_n << std::endl;
        }
    }
}

int main(int argc, char *argv[]) {
    // read_oa();
    // test_oa_box();
    print_tech_info();

    return 0;
}
