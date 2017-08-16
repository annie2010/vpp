/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_VXLAN_TUNNEL_H__
#define __VPP_VXLAN_TUNNEL_H__

#include "object_base.hpp"
#include "om.hpp"
#include "hw.hpp"
#include "rpc_cmd.hpp"
#include "singular_db.hpp"
#include "route.hpp"
#include "route_domain.hpp"
#include "interface.hpp"
#include "inspect.hpp"

#include <vpp/vxlan.api.vapi.hpp>

namespace VPP
{
    /**
     * A representation of a VXLAN Tunnel in VPP
     */
    class vxlan_tunnel: public interface
    {
    public:
        /**
         * Combaintion of attributes that are a unique key
         * for a VXLAN tunnel
         */
        struct endpoint_t
        {
            /**
             * Default constructor
             */
            endpoint_t();
            /**
             * Constructor taking endpoint values
             */
            endpoint_t(const boost::asio::ip::address &src,
                       const boost::asio::ip::address &dst,
                       uint32_t vni);

            /**
             * less-than operator for map storage
             */
            bool operator<(const endpoint_t &o) const;

            /**
             * Comparison operator
             */
            bool operator==(const endpoint_t &o) const;

            /**
             * Debug print function
             */
            std::string to_string() const;

            /**
             * The src IP address of the endpoint
             */
            boost::asio::ip::address src;

            /**
             * The destination IP address of the endpoint
             */
            boost::asio::ip::address dst;

            /**
             * The VNI of the endpoint
             */
            uint32_t vni;
        };

        /**
         * Construct a new object matching the desried state
         */
        vxlan_tunnel(const boost::asio::ip::address &src,
                     const boost::asio::ip::address &dst,
                     uint32_t vni);

        /**
         * Construct a new object matching the desried state with a handle
         * read from VPP
         */
        vxlan_tunnel(const handle_t &hdl,
                     const boost::asio::ip::address &src,
                     const boost::asio::ip::address &dst,
                     uint32_t vni);

        /*
         * Destructor
         */
        ~vxlan_tunnel();

        /**
         * Copy constructor
         */
        vxlan_tunnel(const vxlan_tunnel& o);

        /**
         * Return the matching 'singular instance'
         */
        std::shared_ptr<vxlan_tunnel> singular() const;

        /**
         * Debug rpint function
         */
        virtual std::string to_string() const;

        /**
         * Return VPP's handle to this object
         */
        const handle_t & handle() const;

        /**
         * Dump all L3Configs into the stream provided
         */
        static void dump(std::ostream &os);

        /**
         * A Command class that creates an VXLAN tunnel
         */
        class create_cmd: public interface::create_cmd<vapi::Vxlan_add_del_tunnel>
        {
        public:
            /**
             * Create command constructor taking HW item to update and the
             * endpoint values
             */
            create_cmd(HW::item<handle_t> &item,
                       const std::string &name,
                       const endpoint_t &ep);

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
            bool operator==(const create_cmd&i) const;
        private:
            /**
             * Enpoint values of the tunnel to be created
             */
            const endpoint_t m_ep;
        };

        /**
         * A functor class that creates an VXLAN tunnel
         */
        class delete_cmd: public interface::delete_cmd<vapi::Vxlan_add_del_tunnel>
        {
        public:
            /**
             * delete command constructor taking HW item to update and the
             * endpoint values
             */
            delete_cmd(HW::item<handle_t> &item,
                       const endpoint_t &ep);

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
            bool operator==(const delete_cmd&i) const;
        private:
            /**
             * Enpoint values of the tunnel to be deleted
             */            
            const endpoint_t m_ep;
        };

        /**
         * A cmd class that Dumps all the Vpp interfaces
         */
        class dump_cmd: public VPP::dump_cmd<vapi::Vxlan_tunnel_dump>
        {
        public:
            /**
             * Default Constructor
             */
            dump_cmd();

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
            bool operator==(const dump_cmd&i) const;
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
         * Event handle to register with OM
         */
        static event_handler m_evh;

        /**
         * Commit the acculmulated changes into VPP. i.e. to a 'HW" write.
         */
        void update(const vxlan_tunnel &obj);

        /**
         * Return the matching 'instance' of the sub-interface
         *  over-ride from the base class
         */
        std::shared_ptr<interface> singular_i() const;

        /**
         * Find the VXLAN tunnel in the OM
         */
        static std::shared_ptr<vxlan_tunnel> find_or_add(const vxlan_tunnel &temp);

        /*
         * It's the VPPHW class that updates the objects in HW
         */
        friend class VPP::OM;

        /**
         * It's the VPP::singular_db class that calls replay()
         */
        friend class VPP::singular_db<endpoint_t, vxlan_tunnel>;

        /**
         * Sweep/reap the object if still stale
         */
        void sweep(void);

        /**
         * replay the object to create it in hardware
         */
        void replay(void);

        /**
         * Tunnel enpoint/key
         */
        endpoint_t m_tep;

        /**
         * A map of all VLAN tunnela against thier key
         */
        static singular_db<endpoint_t, vxlan_tunnel> m_db;

        /**
         * Construct a unique name for the tunnel
         */
        static std::string mk_name(const boost::asio::ip::address &src,
                                   const boost::asio::ip::address &dst,
                                   uint32_t vni);
    };

    /**
     * Ostream output for a tunnel endpoint
     */
    std::ostream & operator<<(std::ostream &os, const vxlan_tunnel::endpoint_t &ep);
};
#endif
