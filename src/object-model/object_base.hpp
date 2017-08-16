/*
 * Copyright (c) 2017 Cisco Systems, Inc. and others.  All rights reserved.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License v1.0 which accompanies this distribution,
 * and is available at http://www.eclipse.org/legal/epl-v10.html
 */

#ifndef __VPP_OBJECT_H__
#define __VPP_OBJECT_H__

#include <string>
#include <memory>

#include "types.hpp"

namespace VPP
{
    /**
     * A base class for all object_base in the VPP object_base-Model.
     *  provides the abstract interface.
     */
    class object_base
    {
    public:
        /**
         * convert to string format for debug purposes
         */
        virtual std::string to_string() const = 0;

        /**
         * Sweep/reap the object if still stale
         */
        virtual void sweep(void) = 0;

        /**
         * replay the object to create it in hardware
         */
        virtual void replay(void) = 0;

    protected:
        /**
         * Constructable by derived classes only
         */
        object_base() = default;
        /**
         * Destructor
         */
        virtual ~object_base() = default;

    private:
        /**
         * note we are not maintaining dependencies back to the
         * keys. i.e. this object does not know all the keys that
         * refer to it.
         */
    };

    /**
     * object state
     */
    enum obj_state_t
    {
        OBJECT_STATE_NONE = 0,
        /**
         * indicates the object is stale. This flag is set
         * when a new epoch is declared. the flag is cleared
         * when the object is updated in the new epoch. If the
         * flag is still set after convergence is declared then
         * the object is deleted
         */
        OBJECT_STATE_STALE,
    };

    /**
     * A represenation of a reference to a VPP object.
     *  the reference counting is held through the use of shared pointers.
     * We also maintain the state of the object ready for mark n' sweep.
     */
    class object_ref
    {
    public:
        /**
         * Constructor
         */
        object_ref(std::shared_ptr<object_base> obj);

        /**
         * less than operator
         */
        bool operator<(const object_ref &other) const;

        /**
         * Return the shared pointer
         */
        std::shared_ptr<object_base> obj() const;

        /**
         * Mark the reference object as stale
         */
        void mark()  const;

        /**
         * Clear the stale flag on the object
         */
        void clear() const;

        /**
         * Query if the object is stale
         */
        bool stale() const;

    private:
        /**
         * The reference object
         */
        std::shared_ptr<object_base> m_obj;

        /**
         * Not part of the key (in the set) so we can change it
         * whilst iterating
         */
        mutable obj_state_t m_state;
    };

    /**
     * ostream print of a VPP Obect
     */
    std::ostream& operator<<(std::ostream &os, const object_base& o);
    
};
#endif
