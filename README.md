# OpenMOM-English
English localization of OpenMOM 4.2.0.  Based on the original OpenMOM project. English Edition EN1. Localized byNorifumi Matsukawa.

## 1. Introduction

  1.1 Overview

(1) About This Software
OpenMOM is an electromagnetic field simulator based on the Method of Moments (MoM) .
It can be used for the analysis of antennas and electromagnetic fields.

(2) Open Source License
OpenMOM is open-source free software.
Users are free to use, modify, and redistribute the software.
However, the software is provided without any warranty.

(3) System Requirements
OpenMOM runs on both Windows and Linux.
The Windows package includes a simple graphical user interface (GUI) and executable programs.

(4) Computational Method
The fundamental equations of electromagnetics are transformed into a system of linear equations.
The resulting system is represented by a complex symmetric dense matrix, which is solved using the Modified Cholesky Method.
The computed current distribution is then used to calculate far-field and near-field electromagnetic characteristics.

(5) Output
Computation results are generated as graphical files and numerical data files in text format.
Graphical output is available in both HTML format and a proprietary format.
Under Windows, files in the proprietary format can be displayed using the included viewer application.

(6) Acceleration Techniques
The following acceleration techniques are utilized - SIMD-based vectorization
Shared-memory parallelization using OpenMP.

1.2 Installation

1.2.1 Installing OpenMOM
Download and extract OpenMOM.zip. Copy the extracted OpenMOM folder to a suitable location, such as your Documents directory.
When upgrading to a newer version, overwrite all existing files with the files from the new release.
The folder structure is as follows:

omm.exe - Calculation program (single-precision version)

omm_double.exe - Calculation program (double-precision version)

omm_post.exe - Post-processing program

OpenMOM.exe - GUI application

ev2d.exe - 2D visualization program

ev3d_otk.exe - 3D visualization program (OpenTK version)

vcomp140.dll - OpenMP runtime DLL (Microsoft)

OpenTK.dll - Runtime DLL for ev3d_otk.exe (provided by OpenTK)

OpenTK.GLControl.dll - Runtime DLL for ev3d_otk.exe (provided by OpenTK)

include/ - Header files

sol/ - Source code for calculation programs

post/ - Source code for post-processing programs

data/ - Input data directory

sample/ - Sample data

benchmark/ - Benchmark data

datalib/ - Source code for the data creation library

python/ - Python implementation source code (see Chapter 8)

1.2.2 Notes for Windows Users
Use this software at your own risk. The author accepts no responsibility for any damage or loss resulting from its use.
Before running the application for the first time, perform the following operation for all executable and DLL files listed above (items 1-9):
Right-click the file
Select Properties
Open the General tab
Under Security, click Unblock (or enable Allow)
If this step is skipped, Windows may display the following message:
"This operation has been canceled by the user."
The provided executable programs are built for Windows 11 (64-bit).
No development environment is required to run the executables. However, a C/C++ compiler is required if you wish to modify and rebuild the source code.

1.2.3 Notes for Linux Users
Files with the extensions .exe and .dll are Windows binaries and may be safely removed.
A C/C++ compiler is required to build the source code.
Graphics output is generated in HTML format.
Refer to Sections 4.5 and 4.6 for usage instructions.

1.2.4 Uninstalling OpenMOM
To uninstall OpenMOM, simply delete:
The downloaded archive OpenMOM.zip
The extracted OpenMOM folder
No additional uninstallation procedure is required.

## Attention
For complete documentation, visit:

https://ss023804.stars.ne.jp/OpenMOM/
 
Browser translation is recommended for users who require the full manual.
