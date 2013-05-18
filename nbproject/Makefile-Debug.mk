#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../../../../opencv/build/x86/mingw/lib

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_legacy245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_calib3d245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_ml245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_contrib245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_nonfree245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_core245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_objdetect245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_features2d245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_photo245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_flann245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_stitching245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_gpu245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_ts245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_haartraining_engine.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_video245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_highgui245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_videostab245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: opencv_imgproc245.lib

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} $@.d
	$(COMPILE.cc) -g -I../../../../../opencv/build/include -I../../../../../opencv/build/include/opencv -MMD -MP -MF $@.d -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/difference.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
