/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_EVENT_CMD_H__
#define __VPP_EVENT_CMD_H__

#include <queue>
#include <mutex>

#include "cmd.hpp"

#include <vpp/vapi.hpp>

namespace VPP
{
    /**
     * An Event command base class.
     * Events are one of the sub-set of command type to VPP.
     * A client performs a one time 'registration/subsription' to VPP for the
     * event in question and then is notified asynchronously when those events
     * occur. 
     * The model here then is that the lifetime of the event command represensts
     * the during of the clients subscription. When the command is 'issued' the
     * subscription begins, when it is 'retired' the subscription ends. For the
     * subscription duration the client will be notified as events are recieved.
     * The client can then 'pop' these events from this command object.
     */
    template <typename MSG>
    class event_cmd
    {
    public:
        /**
         * Default constructor
         */
        event_cmd()
        {
        }

        /**
         * Default destructor
         */
        virtual ~event_cmd()
        {
        }

        /**
         * Typedef for the event type
         */
        typedef typename vapi::Event_registration<MSG>::resp_type event_t;
        typedef typename vapi::Event_registration<MSG> reg_t;

        typedef typename vapi::Result_set<typename reg_t::resp_type>::const_iterator const_iterator;

        const_iterator begin()
        {
            return (m_reg->get_result_set().begin());
        }

        const_iterator end()
        {
            return (m_reg->get_result_set().end());
        }

        void lock()
        {
            m_mutex.lock();
        }
        void unlock()
        {
            m_mutex.unlock();
        }

        /**
         * flush/free all the events thus far reeived.
         * Call with the lock held!
         */
        void flush()
        {
            m_reg->get_result_set().free_all_responses();
        }

        /**
         * Retire the command. This is only appropriate for Event Commands
         * As they persist until retired.
         */
        virtual void retire() = 0;

        vapi_error_e operator() (reg_t &dl)
        {
            notify();

            return (VAPI_OK);     
        }

    protected:
        /**
         * Notify the command that data from VPP has arrived and been stored.
         * The command should now inform its clients/listeners.
         */
        virtual void notify() = 0;

        /**
         * The VAPI event registration
         */
        std::unique_ptr<vapi::Event_registration<MSG>> m_reg;

        /**
         * Mutex protection for the events
         */
        std::mutex m_mutex;
    };
};


#endif