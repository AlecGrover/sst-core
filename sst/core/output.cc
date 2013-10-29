// Copyright 2009-2013 Sandia Corporation. Under the terms
// of Contract DE-AC04-94AL85000 with Sandia Corporation, the U.S.
// Government retains certain rights in this software.
// 
// Copyright (c) 2009-2013, Sandia Corporation
// All rights reserved.
// 
// This file is part of the SST software package. For license
// information, see the LICENSE file in the top level directory of the
// distribution.

///////////////////////////////////////
// Include Files per Coding Standards
///////////////////////////////////////
#include "sst_config.h"
#include "sst/core/serialization.h"

#include "sst/core/output.h"

// System Headers
#include <errno.h>

// Core Headers
#include "sst/core/simulation.h"

#ifdef HAVE_MPI
#include <boost/mpi.hpp>
#endif

    
namespace SST {

// Initialize The Static Member Variables     
std::string Output::m_sstFileName = "";
std::FILE*  Output::m_sstFileHandle = 0;
uint32_t    Output::m_sstFileAccessCount = 0;


Output::Output(const std::string& prefix, uint32_t verbose_level,   
               uint32_t verbose_mask,output_location_t location)
{
    m_objInitialized = false;

    init(prefix, verbose_level, verbose_mask, location);
} 


Output::Output()
{
    m_objInitialized = false;

    // Set Member Variables
    m_outputPrefix = "";
    m_verboseLevel = 0;
    m_verboseMask  = 0;;
    m_targetLoc    = NONE;
    m_MPIProcName  = "";      
    m_MPIWorldSize = 0;
    m_MPIWorldRank = 0;

} 


void Output::init(const std::string& prefix, uint32_t verbose_level,  
                  uint32_t verbose_mask, output_location_t location)
{
    // Only initialize if the object has not yet been initialized.
    if (false == m_objInitialized) {
        
        // Set Member Variables
        m_outputPrefix = prefix;
        m_verboseLevel = verbose_level;
        m_verboseMask  = verbose_mask;
        
        setTargetOutput(location);    
        
#ifdef HAVE_MPI
        // Get the MPI Info
        m_MPIProcName  = boost::mpi::environment::processor_name();      
        boost::mpi::communicator MPIWorld;
        m_MPIWorldSize = MPIWorld.size();
        m_MPIWorldRank = MPIWorld.rank();
#endif
    
        m_objInitialized = true;
    }
}


Output::~Output()
{
    // Check to see if we need to close the file
    closeSSTTargetFile();
}


void Output::setVerboseLevel(uint32_t verbose_level)
{
    m_verboseLevel = verbose_level;
}


uint32_t Output::getVerboseLevel() const
{
    return m_verboseLevel;
}


void Output::setVerboseMask(uint32_t verbose_mask)
{
    m_verboseMask = verbose_mask;
}


uint32_t Output::getVerboseMask() const
{
    return m_verboseMask;
}


void Output::setPrefix(const std::string& prefix)
{
    m_outputPrefix = prefix;
}


std::string Output::getPrefix() const
{
    return m_outputPrefix;
}


void Output::setOutputLocation(output_location_t location)
{
    // Check to see if we need to close the file (if we are set to FILE)
    closeSSTTargetFile();
    
    // Set the new target output
    setTargetOutput(location);    
}


Output::output_location_t Output::getOutputLocation() const
{
    return m_targetLoc;
}


void Output::fatal(uint32_t line, const char* file, const char* func,
                   uint32_t exit_code, uint32_t output_level, uint32_t output_bits,
                   const char* format, ...)    const
{
    va_list     arg;
    
    if (true == m_objInitialized) {
        // First check to see if we are allowed to send output based upon the 
        // verbose_mask and verbose_level checks
        if (((output_bits & ~m_verboseMask) == 0) &&
           (output_level <= m_verboseLevel)){
        
            // Get the argument list and then print it out
            va_start(arg, format);
            outputprintf(line, file, func, format, arg);
            va_end(arg);    
        }
    }
    flush();
#ifdef HAVE_MPI
    boost::mpi::environment::abort(exit_code);      
#else
    exit(1);
#endif
}

void Output::setFileName(const std::string& filename)  /* STATIC METHOD */
{
    // This method will be called by the SST core during startup parameter 
    // checking to set the output file name.  It is not inteded to be called 
    // by the SST components.
    //
    // NOTE: This method can be called only once  
    
    // Make sure we do not have a empty string from the user.
    if (0 == filename.length()) {
        // We need to abort here, this is an illegal condition
        printf("ERROR: Output::setFileName(filename) - Parameter filename cannot be an empty string.\n");
        exit(-1);
    }
    
    // Set the Filename, only if it has not yet been set.
    if (0 == m_sstFileName.length()) {
        m_sstFileName = filename;
    } else {
        printf("ERROR: Output::setFileName() - Filename is already set to %s, and canot be changed.\n", m_sstFileName.c_str());
        exit(-1);
    }
}


void Output::setTargetOutput(output_location_t location)
{
    m_targetLoc = location;
    
    // Figure out where we need to send the output, we do this here rather
    // than over and over in the output methods.  If set to NONE, we choose 
    // stdout, but will not output any value (checked in the output methods)
    switch (m_targetLoc) {
    case FILE:
        m_targetOutputRef = &m_sstFileHandle;
        // Increment the Access count for the file
        m_sstFileAccessCount++; 
        break;
    case STDERR:
        m_targetOutputRef = &stderr;
        break;
    case NONE: 
    case STDOUT:
    default:
        m_targetOutputRef = &stdout;
        break;
    }
}


void Output::openSSTTargetFile() const
{
    std::FILE*  handle;
    std::string tempFileName;
    char        tempBuf[256];
    
    if (true == m_objInitialized) {
        // Check to see if the File has not been opened.
        if ((m_sstFileAccessCount > 0) && (0 == m_sstFileHandle)){
            tempFileName = m_sstFileName;
            
            // Append the rank to file name if MPI_COMM_WORLD is GT 1
            if (m_MPIWorldSize > 1){
                sprintf(tempBuf, "%d", m_MPIWorldRank);
                tempFileName += tempBuf;
            }
            
            // Now try to open the file
            handle = fopen(tempFileName.c_str(), "w");
            if (NULL != handle){
                m_sstFileHandle = handle;
            } else {
                // We got an error of some sort
                printf("ERROR: Output::openSSTTargetFile() - Problem opening File %s - %s\n", tempFileName.c_str(), strerror(errno));
                exit(-1);
            }
        }
    }
}


void Output::closeSSTTargetFile()
{
    if (true == m_objInitialized) {
        // Decrement the Access count for the file
        if ((m_sstFileAccessCount > 0) && (FILE == m_targetLoc)){
            m_sstFileAccessCount--; 
        }
        
        // If the access count is zero, and the file has been opened, then close it
        if ((0 == m_sstFileAccessCount) && 
            (0 != m_sstFileHandle) &&
            (FILE == m_targetLoc)){
            fclose (m_sstFileHandle);
        }
    }
}


std::string Output::buildPrefixString(uint32_t line, const std::string& file, const std::string& func) const
{
    std::string rtnstring = "";
    size_t      startindex = 0;
    size_t      findindex = 0;
    char        tempBuf[256];

    // Scan the string for tokens 
    while(std::string::npos != findindex){
        
        // Find the next '@' from the starting index 
        findindex = m_outputPrefix.find("@", startindex);

        // Check to see if we found anything
        if (std::string::npos != findindex){
            
            // We found the @, copy the string up to this point 
            rtnstring += m_outputPrefix.substr(startindex, findindex - startindex);

            // check the next character to see what we need to do
            switch (m_outputPrefix[findindex+1])
            {
            case 'f' :
                rtnstring += file;
                startindex = findindex + 2;
                break;
            case 'l' :
                sprintf(tempBuf, "%d", line);
                rtnstring += tempBuf;
                startindex = findindex + 2;
                break;
            case 'p' :
                rtnstring += func;
                startindex = findindex + 2;
                break;
            case 'r' :
                if (1 == m_MPIWorldSize){
                    rtnstring += "";
                } else {
                    sprintf(tempBuf, "%d", m_MPIWorldRank);
                    rtnstring += tempBuf;
                }
                startindex = findindex + 2;
                break;
            case 'R' :
                if (1 == m_MPIWorldSize){
                    rtnstring += "0";
                } else {
                    sprintf(tempBuf, "%d", m_MPIWorldRank);
                    rtnstring += tempBuf;
                }
                startindex = findindex + 2;
                break;
            case 't' :
                sprintf(tempBuf, "%" PRIu64, Simulation::getSimulation()->getCurrentSimCycle());
                rtnstring += tempBuf;
                startindex = findindex + 2;
                break;

            default :
                // This character is not one of our tokens, so just copy it
                rtnstring += "@";
                startindex = findindex + 1;
                break;
            }
        }
    }
    // copy the remainder of the string from the start index to the end.
    rtnstring += m_outputPrefix.substr(startindex);
    
    return rtnstring;
}


void Output::outputprintf(uint32_t line, const std::string &file,
                          const std::string &func, const char* format, va_list arg) const
{
    std::string newFmt;
    
    // If the target output is a file, Make sure that the file is created and opened
    if ((FILE == m_targetLoc) && (0 == m_sstFileHandle)) {
        openSSTTargetFile();
    }
    
    // Check to make sure output location is not NONE
    if (NONE != m_targetLoc) {
        newFmt = buildPrefixString(line, file, func) + format;
        std::vfprintf(*m_targetOutputRef, newFmt.c_str(), arg);
    }
}


void Output::outputprintf(const char* format, va_list arg) const
{
    // If the target output is a file, Make sure that the file is created and opened
    if ((FILE == m_targetLoc) && (0 == m_sstFileHandle)) {
        openSSTTargetFile();
    }
    
    // Check to make sure output location is not NONE
    if (NONE != m_targetLoc) {
        std::vfprintf(*m_targetOutputRef, format, arg);
    }
}


template<class Archive> void Output::serialize(Archive& ar, const unsigned int version) {
    ar & BOOST_SERIALIZATION_NVP(m_objInitialized);
    ar & BOOST_SERIALIZATION_NVP(m_outputPrefix);
    ar & BOOST_SERIALIZATION_NVP(m_verboseLevel);
    ar & BOOST_SERIALIZATION_NVP(m_verboseMask);
    ar & BOOST_SERIALIZATION_NVP(m_targetLoc);
    ar & BOOST_SERIALIZATION_NVP(m_MPIWorldSize);
    ar & BOOST_SERIALIZATION_NVP(m_MPIWorldRank);
    ar & BOOST_SERIALIZATION_NVP(m_MPIProcName);
}


} // namespace SST

SST_BOOST_SERIALIZATION_INSTANTIATE(SST::Output::serialize)
BOOST_CLASS_EXPORT_IMPLEMENT(SST::Output);

