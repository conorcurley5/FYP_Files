import numpy as np
import math

internal_diameter = 35
external_diameter = 55
spacing = 1

index = 0

def main():
    global index
    print("Generating traces for interal diameter % s, outer diameter % s, spacing: %s" % (internal_diameter, external_diameter, spacing))
    length = 0
    for trace in np.arange(internal_diameter, external_diameter + spacing, spacing):
        circ = (2 * math.pi * trace)/2
        length += circ + spacing 
        print("""
            (gr_arc
                (start -% s 0)
                (mid 0 % s)
                (end %s 0)
                (stroke
                    (width 0.7)
                    (type default)
                )
                (layer "F.Cu")
            )
        """ % (trace, trace, trace))

        if (index == 0):
            print("""
                (segment
                    (start % s 0)
                    (end % s 0)
                    (width 0.7)
                    (layer "F.Cu")
                    (net 0)
                    (uuid "3bbdbe6e-9d70-4bbc-a3d2-3e2505b27e8f")
                )
            """ % (-trace, -trace-spacing))
            index = 1
        else:
            print("""
                (segment
                    (start % s 0)
                    (end % s 0)
                    (width 0.7)
                    (layer "F.Cu")
                    (net 0)
                    (uuid "3bbdbe6e-9d70-4bbc-a3d2-3e2505b27e8f")
                )
            """ % (trace, trace+spacing))
            index = 0
    print(length/1000)


if __name__ == '__main__':
    main()