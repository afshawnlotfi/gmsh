l = .2;
Point(7) = {-1.4, 0.5, 0, l};
Point(8) = {-1.4, -1.1, 0, l};
Point(9) = {1.4, -1.1, 0, l};
Point(10) = {1.4, 0.5, 0, l};
Line(2) = {7, 10};
Line(3) = {9, 10};
Line(4) = {9, 8};
Line(5) = {8, 7};
Line Loop(7) = {2,-3,4,5};
Plane Surface(8) = {7};
Field[1] = BoundaryLayer;
Field[1].CurvesList = {4};
Field[1].SizeFar = 0.1;
Field[1].Size = 0.005;
Field[1].Thickness = 0.1;
Field[1].Ratio = 1.4;
Field[1].PointsList = {8, 9};

BoundaryLayer Field = 1;
