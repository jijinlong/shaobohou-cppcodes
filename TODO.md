# Introduction #

Features and improvements.


# Planned #

Listed in order of priority:
  * The joint epsilon zone of two near coplanar facets can extend quite far from the convex hull itself, causing some points to be prematurely removed by Facet::UpdateOutsideSets. Try retaining any point whose farthest distance from any facet is less than epsilon but greater than zero, and attempt re-add at later stages.
  * Better initialisation of the double-triangle. The initial point set should be processed in linear order, first constructing an edge and then the double-triangle, removing redundant points at each stage.
  * a failed updateFacet() call should only remove the farthestPoint (add it to tempPoints?) and not clear the outside set
  * Better implementation of 2d convex hull using the quickhull algorithm
  * Implement the rotating caliper method for determining OBB from convex hulls (2D & 3D)
  * Better implementation of ConvexHull3D::distance2hull() which takes into account of edges and vertices
  * Better subdivision of triangle set for constructing OBB tree
  * Clean up collision detection (GJK) and collision response code


# Done #