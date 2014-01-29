render(2) difference() {
	cube([47.15,57.95,.3]);
	minkowski() { 
		translate([3,3,0]) linear_extrude(.2) import ("charlie heart_top_paste.dxf");
		cube(.2);
	}
}
