/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_CONTROL_INTERFACE_H__
#define __VPP_CONTROL_INTERFACE_H__

#include "interface.hpp"
#include "route.hpp"

namespace VPP
{
    /**
     * A tap-interface. e.g. a tap interface
     */
    class tap_interface: public interface
    {
    public:
        
        tap_interface(const std::string &name,
                      admin_state_t state,
                      route::prefix_t prefix);

        tap_interface(const std::string &name,
                      admin_state_t state,
                      route::prefix_t prefix,
                      const l2_address_t &l2_address);

        ~tap_interface();
        tap_interface(const tap_interface& o);

        /**
         * Return the matching 'singular instance' of the TAP interface
         */
        std::shared_ptr<tap_interface> singular() const;

        /**
         * A functor class that creates an interface
         */
        class create_cmd: public interface::create_cmd<vapi::Tap_connect>
        {
        public:
            create_cmd(HW::item<handle_t> &item,
                       const std::string &name,
                       route::prefix_t &prefix,
                       const l2_address_t &l2_address);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

        private:
            route::prefix_t &m_prefix;
            const l2_address_t &m_l2_address;
        };

        /**
         * 
         */
        class delete_cmd: public interface::delete_cmd<vapi::Tap_delete>
        {
        public:
            delete_cmd(HW::item<handle_t> &item);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;
        };

    private:
        /**
         * Construct with a handle
         */
        tap_interface(const handle_t &hdl,
                      const std::string &name,
                      admin_state_t state,
                      route::prefix_t prefix);

        /**
         * Ip Prefix
         */
        route::prefix_t m_prefix;

        l2_address_t m_l2_address;

        /**
         * interface is a friend so it can construct with handles
         */
        friend class interface;

        /**
         * Return the matching 'instance' of the sub-interface
         *  over-ride from the base class
         */
        std::shared_ptr<interface> singular_i() const;

        /**
         * Virtual functions to construct an interface create commands.
         */
        virtual std::queue<cmd*> & mk_create_cmd(std::queue<cmd*> &cmds);

        /**
         * Virtual functions to construct an interface delete commands.
         */
        virtual std::queue<cmd*> & mk_delete_cmd(std::queue<cmd*> &cmds);

        /*
         * It's the VPP::OM class that call singular()
         */
        friend class VPP::OM;
    };

}

#endif