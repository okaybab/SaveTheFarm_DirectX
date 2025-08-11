/*
* Copyright (c) 2006-2009 Erin Catto http://www.gphysics.com
*
* Permission to use, copy, modify, distribute and sell this software
* and its documentation for any purpose is hereby granted without fee,
* provided that the above copyright notice appear in all copies.
* Erin Catto makes no representations about the suitability 
* of this software for any purpose.  
* It is provided "as is" without express or implied warranty.
*/

#include "box2d-lite/World.h"
#include "box2d-lite/Body.h"
#include "box2d-lite/Joint.h"

using std::vector;
using std::map;
using std::pair;

typedef map<ArbiterKey, Arbiter>::iterator ArbIter;
typedef pair<ArbiterKey, Arbiter> ArbPair;

bool World::accumulateImpulses = true;
bool World::warmStarting = true;
bool World::positionCorrection = true;

void World::Add(Body* body)
{
	if (body == nullptr) return;

	// 중복 추가 방지
	auto it = std::find(bodies.begin(), bodies.end(), body);
	if (it == bodies.end())
	{
		bodies.push_back(body);
	}

	validBodies.insert(body);
}

void World::Remove(Body* body)
{
	if (body == nullptr) return;

	// bodies 벡터에서 제거
	auto bodyIt = std::find(bodies.begin(), bodies.end(), body);
	if (bodyIt != bodies.end())
	{
		bodies.erase(bodyIt);
	}

	// 관련된 Arbiter들 제거
	auto arbiterIt = arbiters.begin();
	while (arbiterIt != arbiters.end())
	{
		if (arbiterIt->first.body1 == body || arbiterIt->first.body2 == body)
		{
			arbiterIt = arbiters.erase(arbiterIt);
		}
		else
		{
			++arbiterIt;
		}
	}

	// 관련된 Joint들 제거 (선택사항)
	auto jointIt = joints.begin();
	while (jointIt != joints.end())
	{
		if ((*jointIt)->body1 == body || (*jointIt)->body2 == body)
		{
			jointIt = joints.erase(jointIt);
		}
		else
		{
			++jointIt;
		}
	}

	validBodies.erase(body);
}

void World::Add(Joint* joint)
{
	if (joint == nullptr) return;

	// 중복 추가 방지
	auto it = std::find(joints.begin(), joints.end(), joint);
	if (it == joints.end())
	{
		joints.push_back(joint);
	}

	validjoints.insert(joint);
}

void World::Remove(Joint* joint)
{
	if (joint == nullptr) return;

	// joints 벡터에서 제거
	auto jointIt = std::find(joints.begin(), joints.end(), joint);
	if (jointIt != joints.end())
	{
		joints.erase(jointIt);
	}

	validjoints.erase(joint);
}
void World::Clear()
{
	bodies.clear();
	joints.clear();
	arbiters.clear();
}

void World::BroadPhase()
{
	// O(n^2) broad-phase
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* bi = bodies[i];

		for (int j = i + 1; j < (int)bodies.size(); ++j)
		{
			Body* bj = bodies[j];

			if (bi->invMass == 0.0f && bj->invMass == 0.0f)
				continue;

			if (bi->isTrigger || bj->isTrigger)
			{
				Arbiter MSGArb(bi, bj);
				ArbiterKey key(bi, bj);

				MSGArb.isMSGOnly = true;

				continue;
			}
				

			Arbiter newArb(bi, bj);
			ArbiterKey key(bi, bj);

			if (newArb.numContacts > 0)
			{
				ArbIter iter = arbiters.find(key);
				if (iter == arbiters.end())
				{
					arbiters.insert(ArbPair(key, newArb));
				}
				else
				{
					iter->second.Update(newArb.contacts, newArb.numContacts);
				}
			}
			else
			{
				arbiters.erase(key);
			}
		}
	}
}

void World::Step(float dt)
{
	float inv_dt = dt > 0.0f ? 1.0f / dt : 0.0f;

	// Determine overlapping bodies and update contact points.
	BroadPhase();

	// Integrate forces.
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* b = bodies[i];

		if (b->invMass == 0.0f)
			continue;

		b->velocity += dt * (gravity + b->invMass * b->force);
		b->angularVelocity += dt * b->invI * b->torque;
	}

	// Perform pre-steps.
	for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb)
	{
		if (arb->second.isMSGOnly)
			continue;

		arb->second.PreStep(inv_dt);
	}

	for (int i = 0; i < (int)joints.size(); ++i)
	{
		joints[i]->PreStep(inv_dt);	
	}

	// Perform iterations
	for (int i = 0; i < iterations; ++i)
	{
		for (ArbIter arb = arbiters.begin(); arb != arbiters.end(); ++arb)
		{
			if (arb->second.isMSGOnly)
				continue;

			arb->second.ApplyImpulse();
		}

		for (int j = 0; j < (int)joints.size(); ++j)
		{
			joints[j]->ApplyImpulse();
		}
	}

	// Integrate Velocities
	for (int i = 0; i < (int)bodies.size(); ++i)
	{
		Body* b = bodies[i];

		b->position += dt * b->velocity;
		b->rotation += dt * b->angularVelocity;

		b->force.Set(0.0f, 0.0f);
		b->torque = 0.0f;
	}
}
