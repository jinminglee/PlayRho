/*
* Original work Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
* Modified work Copyright (c) 2016 Louis Langholtz https://github.com/louis-langholtz/Box2D
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_ISLAND_H
#define B2_ISLAND_H

#include <Box2D/Common/Math.h>
#include <Box2D/Common/AllocatedArray.hpp>
#include <Box2D/Dynamics/TimeStep.h>

namespace box2d {

class Body;
class Contact;
class Joint;
class StackAllocator;
class ContactListener;
struct ContactVelocityConstraint;
struct Profile;

/// Island.
/// @detail A container of bodies contacts and joints relavent to handling world dynamics.
/// @note This is an internal class.
class Island
{
public:
	using BodyContainer = AllocatedArray<Body*, StackAllocator&>;
	using ContactContainer = AllocatedArray<Contact*, StackAllocator&>;
	using JointContainer = AllocatedArray<Joint*, StackAllocator&>;
	using VelocityContainer = AllocatedArray<Velocity, StackAllocator&>;
	using PositionContainer = AllocatedArray<Position, StackAllocator&>;
	
	Island(body_count_t bodyCapacity, contact_count_t contactCapacity, island_count_t jointCapacity,
		   StackAllocator& allocator, ContactListener* listener);

	/// Destructor.
	/// @detail Sets all bodies's island indexes to Body::InvalidIslandIndex and then frees allocated memory.
	~Island();

	/// Solves this island.
	/// @detail This:
	///   1. Updates every body's sweep.pos0 to its sweep.pos1.
	///   2. Updates every body's sweep.pos1 to the new "solved" position for it.
	///   3. Updates every body's velocity to the new accelerated, dampened, and "solved" velocity for it.
	///   4. Synchronizes every body's transform (by updating it to transform one of the body's sweep).
	///   5. Reports to the listener.
	/// @return True if the contact and joint constraints were solved, false otherwise.
	bool Solve(const TimeStep& step, const Vec2& gravity, bool allowSleep);

	/// Solves the time of impact for the two bodies identified by the given island indexes.
	/// @detail This:
	///   1. Updates pos0 of the sweeps of the two bodies identified by their indexes.
	///   2. Updates pos1 of the sweeps, the transforms, and the velocities of the other bodies in this island.
	/// @detail Preconditions:
	///   m_bodies contains the two bodies specified by indexA and indexB;
	///   m_bodies contains appropriate other bodies of the contacts of the two bodies;
	///   m_contacts contains the contact that specified the two identified bodies;
	///   m_contacts contains appropriate other contacts of the two bodies.
	/// @param subStep Sub step time step information.
	/// @param indexA Island index for body A.
	/// @param indexB Island index for body B.
	void SolveTOI(const TimeStep& subStep, island_count_t indexA, island_count_t indexB);

	BodyContainer m_bodies;
	ContactContainer m_contacts;
	JointContainer m_joints;
	
private:
	/// Copy's the position and velocity elements out to the bodies.
	/// @detail This basically flushes out internal position and velocity data to all the bodies in this island
	///   and synchronizes those bodies transformations with their new sweeps.
	/// In specific, this updates this island's bodies' velocities, sweep position 1, and transforms by:
	///    1. setting the velocities to the matching velocity element,
	///    2. setting the sweep position 1 value to the matching position element, and
	///    3. synchronizing the transform with the new sweep value.
	static void CopyOut(const Position* positions, const Velocity* velocities, BodyContainer& bodies);
	
	void InitJointVelocityConstraints(const SolverData& solverData);
	void SolveJointVelocityConstraints(const SolverData& solverData);
	bool SolveJointPositionConstraints(const SolverData& solverData);

	float_t UpdateSleepTimes(float_t h);

	/// Reports the given constraints to the listener.
	/// This calls the listener's PostSolve method for all m_contactCount elements of the given array of constraints.
	/// @param constraints Array of m_contactCount contact velocity constraint elements.
	void Report(const ContactVelocityConstraint* constraints);
		
	StackAllocator& m_allocator; ///< Stack-style memory allocator set on construction.
	ContactListener* const m_listener;
};

inline bool IsFullOfBodies(const Island& island)
{
	return island.m_bodies.size() == island.m_bodies.max_size();
}

inline bool IsFullOfContacts(const Island& island)
{
	return island.m_contacts.size() == island.m_contacts.max_size();
}
	
} // namespace box2d

#endif
