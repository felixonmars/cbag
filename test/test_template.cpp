//
// Created by erichang on 6/10/18.
//

#include <cbag/layout/templates.h>


int main() {
    cbag::CTemplate temp;
    temp.add_rect(1, 0, 0, 0, 20, 10);
    cbag::CRect *r2 = temp.add_rect(2, 0, 20, 0, 20, 30);
    temp.add_rect(1, 0, 0, 0, 5, 20);

    temp.add_pin(1, 1, 0, 0, 10, 10, "foo", "foo");
    cbag::CPin *p2 = temp.add_pin(2, 1, 20, 0, 20, 30, "bar", "bar");

    r2->yh = 40;
    p2->label = "baz";

    std::cout << temp;
}
