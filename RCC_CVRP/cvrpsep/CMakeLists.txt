target_sources(cvrpsep
        PRIVATE basegrph.cpp binpack.cpp blocks.cpp brnching.cpp capsep.cpp cnstrmgr.cpp combsep.cpp compcuts.cpp compress.cpp cutbase.cpp fcapfix.cpp fcisep.cpp fcits.cpp glmsep.cpp grsearch.cpp hpmstar.cpp htoursep.cpp intap.cpp memmod.cpp mstarsep.cpp mxf.cpp newhtour.cpp sort.cpp strcomb.cpp strngcmp.cpp twomatch.cpp
        PUBLIC FILE_SET HEADERS
        BASE_DIRS ${PROJECT_SOURCE_DIR}
        FILES basegrph.h binpack.h blocks.h brnching.h capsep.h cnstrmgr.h combsep.h compcuts.h compress.h cutbase.h fcapfix.h fcisep.h fcits.h glmsep.h grsearch.h hpmstar.h htoursep.h intap.h memmod.h mstarsep.h mxf.h newhtour.h sort.h strcomb.h strngcmp.h tolerances.h twomatch.h)