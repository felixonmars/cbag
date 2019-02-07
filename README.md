# C++ extensions for Berkeley Analog Generator Framework

## BAG3 restrictions

### Schematic

1. All schematic pins must be either a scalar net or a bus net, and if it is a bus net,
   the step size must be exactly 1.  For example, `foo`, `bar<3:0>`, and `baz<0:2>` are valid
   pin names, but `a,b`, `c<3:0:2>`, `<*3>d` are illegal.

2. If a schematic contains a bus pin, then there can be no other net inside the schematic with the
   same base name.  For example, if a cellview has a pin called `foo<3:1>`, you cannot have an
   internal net named `foo<4>`, or `foo<0>`.

## Notes

1. These extensions will only work on architectures that performs arithmetic right shift.  In other
   words, right shift on negative numbers will fill in 1s on the most significant bit.
