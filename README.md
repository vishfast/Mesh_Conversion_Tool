# Mesh_Conversion_Tool (Serial code)

Description:

> Converts a 3D tetrahedral SU2 mesh (or a Gmsh format converted to SU2) to a CAS format for use with Fluent/JOE
  - Take the SU2 file and remove any sections that do not belong to the following:
    1. NDIME
    2. NELEM
    3. NPOIN
    4. NMARK
    5. MARKER_TAG
    6. MARKER_ELEMS
  - Remove any marker surface that do not constitute the boundary
  - Run on the terminal: <executable/file/after/compilation> <SU2/input/file/name> <CAS/output/file/name>
  - Edit the boundary conditions in the cas file
> Rate of mesh conversion on a standard Intel Xeon: ~100,000 cells/second
