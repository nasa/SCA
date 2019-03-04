# Stored Command Absolute 

NASA Core Flight System Stored Command Absolute (SCA) Application

## Description

The Stored Command Absolute (SCA) application is a core Flight System (cFS) application that is a plug in to 
the Core Flight Executive (cFE) component of the cFS.

The cFS is a platform and project independent reusable software framework and set of reusable applications developed 
by NASA Goddard Space Flight Center. This framework is used as the basis for the flight software for satellite data systems 
and instruments, but can be used on other embedded systems. More information on the cFS can be found at 
[http://cfs.gsfc.nasa.gov](http://cfs.gsfc.nasa.gov)

The SCA application allows a system to be commanded 24 hours a day using sequences of absolute time tagged
sequences.  Each command has a time tag associated with it, permitting the command to be released for distribution at
predetermined times.  SCA allows 5 absolute time tagged command sequences to be run concurrently.  Unlike the Stored Command
(SC) application, SCA relies on a text-based format to specify command sequences.  This application is a prototype.

## License

This software is licensed under the NASA Open Source Agreement. http://ti.arc.nasa.gov/opensource/nosa