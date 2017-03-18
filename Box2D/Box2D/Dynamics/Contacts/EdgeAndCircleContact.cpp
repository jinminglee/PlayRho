/*
 * Original work Copyright (c) 2006-2010 Erin Catto http://www.box2d.org
 * Modified work Copyright (c) 2017 Louis Langholtz https://github.com/louis-langholtz/Box2D
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

#include <Box2D/Dynamics/Contacts/EdgeAndCircleContact.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Collision/CollideShapes.hpp>
#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>

#include <new>

using namespace box2d;

Contact* EdgeAndCircleContact::Create(Fixture* fixtureA, child_count_t,
									  Fixture* fixtureB, child_count_t)
{
	return new EdgeAndCircleContact(fixtureA, fixtureB);
}

void EdgeAndCircleContact::Destroy(Contact* contact)
{
	delete static_cast<EdgeAndCircleContact*>(contact);
}

EdgeAndCircleContact::EdgeAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
: Contact{fixtureA, 0, fixtureB, 0}
{
	assert(GetType(*fixtureA) == Shape::e_edge);
	assert(GetType(*fixtureB) == Shape::e_circle);
}

Manifold EdgeAndCircleContact::Evaluate() const
{
	const auto fixtureA = GetFixtureA();
	const auto fixtureB = GetFixtureB();
	const auto xfA = fixtureA->GetBody()->GetTransformation();
	const auto xfB = fixtureB->GetBody()->GetTransformation();
	return CollideShapes(*static_cast<const EdgeShape*>(fixtureA->GetShape()), xfA,
						 *static_cast<const CircleShape*>(fixtureB->GetShape()), xfB);
}
