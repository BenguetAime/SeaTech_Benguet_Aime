The sub-folders in this folder contain the original out-of-box demo .hex
file images that would typically come pre-programmed on the respective Microchip USB
demo board when it originally ships from the factory.  

Some of these sub-folders also contain an archive of the original source code that was
used to create the out-of-box demo firmware.  Normally, the out of box firmware would be
based closely on, or may be identical to, one of the existing USB MLA related demo projects in
the [MLA installed folder]\apps\[project name] folder.

When developing a new USB application, it is recommended to start from the source code in the standard
version of the project under the [MLA installed folder]\apps\[project name] folder, rather than from the
archived firmware soure code in these sub-folders, as the archived code is not updated with each MLA release.
It is therefore possible for the archive copy to be out of date or otherwise be missing new features or bug
fixes that may have already been implemented in the standard version of the project (ex: the projects under 
the [MLA installed folder]\apps\[project name] folder area).