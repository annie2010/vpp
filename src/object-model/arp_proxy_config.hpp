/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_ARP_PROXY_CONFIG_H__
#define __VPP_ARP_PROXY_CONFIG_H__

#include <string>
#include <map>
#include <vector>
#include <stdint.h>

#include "object_base.hpp"
#include "om.hpp"
#include "hw.hpp"
#include "rpc_cmd.hpp"
#include "dump_cmd.hpp"
#include "singular_db.hpp"
#include "inspect.hpp"

#include <vpp/vpe.api.vapi.hpp>

namespace VPP
{
    /**
     * A representation of LLDP client configuration on an interface
     */
    class arp_proxy_config: public object_base
    {
    public:
        /**
         * Key type
         */
        typedef std::pair<boost::asio::ip::address_v4,
                          boost::asio::ip::address_v4> key_t;

        /**
         * Construct a new object matching the desried state
         */
        arp_proxy_config(const boost::asio::ip::address_v4 &low,
                         const boost::asio::ip::address_v4 &high);

        /**
         * Copy Constructor
         */
        arp_proxy_config(const arp_proxy_config& o);

        /**
         * Destructor
         */
        ~arp_proxy_config();

        /**
         * Return the 'singular' of the LLDP config that matches this object
         */
        std::shared_ptr<arp_proxy_config> singular() const;

        /**
         * convert to string format for debug purposes
         */
        std::string to_string() const;

        /**
         * Dump all LLDP configs into the stream provided
         */
        static void dump(std::ostream &os);

        /**
         * A command class that adds the ARP Proxy config
         */
        class config_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                         vapi::Proxy_arp_add_del>
        {
        public:
            /**
             * Constructor
             */
            config_cmd(HW::item<bool> &item,
                       const boost::asio::ip::address_v4 &lo,
                       const boost::asio::ip::address_v4 &high);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

            /**
             * Comparison operator - only used for UT
             */
            bool operator==(const config_cmd&i) const;
        private:
            /**
             * Address range
             */
            const boost::asio::ip::address_v4 m_low;
            const boost::asio::ip::address_v4 m_high;
        };

        /**
         * A cmd class that Unconfigs ArpProxy Config from an interface
         */
        class unconfig_cmd: public rpc_cmd<HW::item<bool>, rc_t,
                                           vapi::Proxy_arp_add_del>
        {
        public:
            /**
             * Constructor
             */
            unconfig_cmd(HW::item<bool> &item,
                         const boost::asio::ip::address_v4 &lo,
                         const boost::asio::ip::address_v4 &hig);

            /**
             * Issue the command to VPP/HW
             */
            rc_t issue(connection &con);
            /**
             * convert to string format for debug purposes
             */
            std::string to_string() const;

            /**
             * Comparison operator - only used for UT
             */
            bool operator==(const unconfig_cmd&i) const;
        private:
            /**
             * Address range
             */
            const boost::asio::ip::address_v4 m_low;
            const boost::asio::ip::address_v4 m_high;
        };

    private:
        /**
         * Class definition for listeners to OM events
         */
        class event_handler: public OM::listener, public inspect::command_handler
        {
        public:
            event_handler();
            virtual ~event_handler() = default;

            /**
             * Handle a populate event
             */
            void handle_populate(const client_db::key_t & key);

            /**
             * Handle a replay event
             */
            void handle_replay();

            /**
             * Show the object in the Singular DB
             */
            void show(std::ostream &os);

            /**
             * Get the sortable Id of the listener
             */
            dependency_t order() const;
        };

        /**
         * event_handler to register with OM
         */
        static event_handler m_evh;

        /**
         * Enquue commonds to the VPP command Q for the update
         */
        void update(const arp_proxy_config &obj);

        /**
         * Find or add LLDP config to the OM
         */
        static std::shared_ptr<arp_proxy_config> find_or_add(const arp_proxy_config &temp);

        /*
         * It's the VPP::OM class that calls singular()
         */
        friend class VPP::OM;

        /**
         * It's the VPP::singular_db class that calls replay()
         */
        friend class VPP::singular_db<arp_proxy_config::key_t, arp_proxy_config>;

        /**
         * Sweep/reap the object if still stale
         */
        void sweep(void);

        /**
         * replay the object to create it in hardware
         */
        void replay(void);

        /**
         * Address range
         */
        const boost::asio::ip::address_v4 m_low;
        const boost::asio::ip::address_v4 m_high;

        /**
         * A map of all ArpProxy configs keyed against the interface.
         */
        static singular_db<arp_proxy_config::key_t, arp_proxy_config> m_db;

        /**
         * HW configuration for the config. The bool representing the
         * do/don't configured/unconfigured.
         */
        HW::item<bool> m_config;
    };

    std::ostream & operator<<(std::ostream &os, const arp_proxy_config::key_t &key);
};

#endif