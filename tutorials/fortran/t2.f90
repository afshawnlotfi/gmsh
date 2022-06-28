! ---------------------------------------------------------------------------
!
!   Gmsh Fortran tutorial 2
!
!   Transformations, extruded geometries, volumes
!
! ---------------------------------------------------------------------------


program main
    use, intrinsic :: iso_c_binding
    use gmsh

    implicit none
    type(gmsh_t) :: gmsh
    integer(c_int) :: t(2,1), numElements(0), ss(2,8), ierr, itmp, i
    real (c_double) :: lc, iv0(0), heights(0)
    integer(c_int), allocatable :: ov(:,:), ov2(:,:)
    character(len=GMSH_API_MAX_STR_LEN), allocatable :: argv(:)
    character(len=GMSH_API_MAX_STR_LEN) :: buf
    real(c_double), allocatable :: xyz(:)

    allocate(argv(command_argument_count() + 1))
    do i = 0, size(argv) - 1
        call get_command_argument(i, buf)
        argv(i+1) = trim(buf)
    end do
    call gmsh%initialize(argv, 1, 0, ierr)

    call gmsh%model%add("t2", ierr)

    ! Copied from `t1.f90'...
    lc = 0.01
    itmp = gmsh%model%geo%addPoint(0.0d0, 0.0d0, 0.0d0, lc, 1, ierr)
    itmp = gmsh%model%geo%addPoint(0.1d0, 0.0d0, 0.0d0, lc, 2, ierr)
    itmp = gmsh%model%geo%addPoint(0.1d0, 0.3d0, 0.0d0, lc, 3, ierr)
    itmp = gmsh%model%geo%addPoint(0.0d0, 0.3d0, 0.0d0, lc, 4, ierr)

    itmp = gmsh%model%geo%addLine(1, 2, 1, ierr)
    itmp = gmsh%model%geo%addLine(3, 2, 2, ierr)
    itmp = gmsh%model%geo%addLine(3, 4, 3, ierr)
    itmp = gmsh%model%geo%addLine(4, 1, 4, ierr)

    itmp = gmsh%model%geo%addCurveLoop([4, 1, -2, 3], 1, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([1], 1, ierr)

    call gmsh%model%geo%synchronize(ierr)

    itmp = gmsh%model%addPhysicalGroup(1, [1, 2, 4], 5, "", ierr)
    itmp = gmsh%model%addPhysicalGroup(2, [1], -1, "My surface", ierr)
    call gmsh%model%setPhysicalName(2, itmp, "My surface", ierr)

    itmp = gmsh%model%geo%addPoint(0.0d0, 0.4d0, 0.0d0, lc, 5, ierr)
    itmp = gmsh%model%geo%addLine(4, 5, 5, ierr)

    t = reshape([0, 5], shape(t))
    call gmsh%model%geo%translate(t, -0.02d0, 0.0d0, 0.0d0, ierr)
    call gmsh%model%geo%rotate(t, 0.0d0, 0.3d0, 0.0d0, &
                              0.0d0, 0.0d0, 1.0d0, -acos(1.0d0) / 4.0d0, ierr)

    ! Note that there are no units in Gmsh: coordinates are just numbers - it's
    ! up to the user to associate a meaning to them.

    ! Point 3 can be duplicated and translated by 0.05 along the y axis by using
    ! the `copy()' function, which takes a vector of (dim, tag) pairs as input,
    ! and returns another vector of (dim, tag) pairs:
    call gmsh%model%geo%Copy(reshape([0, 3], [2, 1]), ov, ierr)
    call gmsh%model%geo%Translate(ov, 0.0d0, 0.05d0, 0.0d0, ierr)

    ! The new point tag is available in ov[0].second, and can be used to create
    ! new lines:
    itmp = gmsh%model%geo%addLine(3, ov(2,1), 7, ierr)
    itmp = gmsh%model%geo%addLine(ov(2,1), 5, 8, ierr)

    itmp = gmsh%model%geo%addCurveLoop([5, -8, -7, 3], 10, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([10], 11, ierr)

    ! In the same way, we can translate copies of the two surfaces 1 and 11 to
    ! the right with the following command:
    deallocate(ov)
    call gmsh%model%geo%Copy(reshape([2, 1, 2, 11], [2, 2]), ov, ierr)
    call gmsh%model%geo%Translate(ov, 0.12d0, 0d0, 0d0, ierr)

    print *, "New surfaces ", ov(2,1), " and ", ov(2,2)

    ! Volumes are the fourth type of elementary entities in Gmsh. In the same way
    ! one defines curve loops to build surfaces, one has to define surface loops
    ! (i.e. `shells') to build volumes. The following volume does not have holes
    ! and thus consists of a single surface loop:
    itmp = gmsh%model%geo%addPoint(0.0d0, 0.30d0, 0.12d0, lc, 100, ierr)
    itmp = gmsh%model%geo%addPoint(0.1d0, 0.30d0, 0.12d0, lc, 101, ierr)
    itmp = gmsh%model%geo%addPoint(0.1d0, 0.35d0, 0.12d0, lc, 102, ierr)

    ! We would like to retrieve the coordinates of point 5 to create point 103,
    ! so we synchronize the model, and use `getValue()'
    call gmsh%model%geo%Synchronize(ierr)

    call gmsh%model%getValue(0, 5, iv0, xyz, ierr)
    itmp = gmsh%model%geo%addPoint(xyz(1), xyz(2), 0.12d0, lc, 103, ierr)
    itmp = gmsh%model%geo%addLine(4, 100, 110, ierr)
    itmp = gmsh%model%geo%addLine(3, 101, 111, ierr)
    itmp = gmsh%model%geo%addLine(6, 102, 112, ierr);
    itmp = gmsh%model%geo%addLine(5, 103, 113, ierr)
    itmp = gmsh%model%geo%addLine(103, 100, 114, ierr)
    itmp = gmsh%model%geo%addLine(100, 101, 115, ierr)
    itmp = gmsh%model%geo%addLine(101, 102, 116, ierr)
    itmp = gmsh%model%geo%addLine(102, 103, 117, ierr)

    itmp = gmsh%model%geo%addCurveLoop([115, -111, 3, 110], 118, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([118], 119, ierr)

    itmp = gmsh%model%geo%addCurveLoop([111, 116, -112, -7], 120, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([120], 121, ierr)

    itmp = gmsh%model%geo%addCurveLoop([112, 117, -113, -8], 122, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([122], 123, ierr)

    itmp = gmsh%model%geo%addCurveLoop([114, -110, 5, 113], 124, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([124], 125, ierr)

    itmp = gmsh%model%geo%addCurveLoop([115, 116, 117, 114], 126, 0, ierr)
    itmp = gmsh%model%geo%addPlaneSurface([126], 127, ierr)

    itmp = gmsh%model%geo%addSurfaceLoop([127, 119, 121, 123, 125, 11], 128, ierr)
    itmp = gmsh%model%geo%addVolume([128], 129, ierr)

    ! When a volume can be extruded from a surface, it is usually easier to use
    ! the `extrude()' function directly instead of creating all the points,
    ! curves and surfaces by hand. For example, the following command extrudes
    ! the surface 11 along the z axis and automatically creates a new volume (as
    ! well as all the needed points, curves and surfaces). As expected, the
    ! function takes a vector of (dim, tag) pairs as input as well as the
    ! translation vector, and returns a vector of (dim, tag) pairs as output:
    call gmsh%model%geo%extrude(reshape(ov(:,2), [2,1]), 0.0d0, 0.0d0, 0.12d0, &
                                ov2, numElements, heights, 0, ierr)

    ! Mesh sizes associated to geometrical points can be set by passing a vector
    ! of (dim, tag) pairs for the corresponding points:
    ss = reshape([0, 103, 0, 105, 0, 109, 0, 102, 0, 28, 0, 24, 0, 6, 0, 5], shape(ss))
    call gmsh%model%geo%mesh%setSize(ss, lc * 3, ierr)

    ! We finish by synchronizing the data from the built-in CAD kernel with the
    ! Gmsh model:
    call gmsh%model%geo%Synchronize(ierr)

    ! We group volumes 129 and 130 in a single physical group with tag `1' and
    ! name "The volume":
    itmp = gmsh%model%addPhysicalGroup(3, [129, 130], 1, "", ierr)

    call gmsh%model%setPhysicalName(3, 1, "The volume", ierr)
    ! We finally generate and save the mesh:
    call gmsh%model%mesh%generate(3, ierr)
    call gmsh%write("t2.msh", ierr)
    ! call gmsh%fltk%run(ierr)
    call gmsh%finalize(ierr)

end program main
