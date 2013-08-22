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


#ifndef SST_INTROSPECTACTION_H
#define SST_INTROSPECTACTION_H


//#include <sst/core/debug.h>
#include <sst/core/action.h>
#include <sst/core/event.h>
#include <sst/core/serialization.h>

namespace SST {

class IntrospectAction : public Action {
public:
    IntrospectAction(Event::HandlerBase* handler) : Action(),
	m_handler( handler ) 
    {
	setPriority(30);
    }
    virtual ~IntrospectAction(){}

    void execute(void);

    
private:
    IntrospectAction() { } // for serialization only
    Event::HandlerBase* m_handler;

    friend class boost::serialization::access;
    template<class Archive>
    void
    serialize(Archive & ar, const unsigned int version );
    
};

}

BOOST_CLASS_EXPORT_KEY(SST::IntrospectAction)

#endif // SST_INTROSPECTACTION_H
