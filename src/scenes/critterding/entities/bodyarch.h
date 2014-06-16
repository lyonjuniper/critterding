#ifndef BODYARCH_H
#define BODYARCH_H

#include "src/common/be_rand.h"
#include "src/common/be_parser.h"
#include "src/utils/settings.h"
#include "src/graphics/be_color.h"
#include "archbodypart.h"
#include "archconstraint.h"
#include "archmouth.h"
#include <vector>
#include <iostream>
#include <sstream>

using namespace std;

class BodyArch
{
	public:
		BodyArch();
		~BodyArch();

		// load save architecture (serialize)
		void			buildArch();
		void			setArch(string* content);
		string*			getArch();
		void			copyFrom(const BodyArch* otherBody);
		void			mutate(const unsigned int runs);

		vector<archBodypart>	archBodyparts;
		vector<archConstraint>	archConstraints;
		vector<archMouth>	archMouths;
		BeColor			color;
		unsigned int		retinasize;

		float			totalWeight;

		int			findBodypart(const unsigned int id);
		int			findMouth( const unsigned int id );
	private:
		BeParser		parseH;
		BeParser		contentParser;
		BeRand			*randgen;
		Settings		*settings;

		float			bodypartspacer;
		void			repositiontoConstraints( archBodypart* bp );
		void			repositiontoConstraints( archMouth* bp );
		void			repositionConstraint( archConstraint* co, const unsigned int oneOrTwo, const btVector3& position );
		void			repositionConstraintAngle( archConstraint* co );
		// mutation helpers
		void			addRandomMouth();
		void			addRandomBodypart();
		void			addRandomConstraint(/*unsigned int connID1, */const unsigned int connID2, bool isMouth);
		void			removeBodypart(const unsigned int id);
		void			removeMouth(const unsigned int id);
		void			randomConstraintPosition(archConstraint* co, const unsigned int OneOrTwo, const unsigned int connID);

		const unsigned int		getUniqueBodypartID();
		const unsigned int		getUniqueConstraintID();

		string			archBuffer;
		const float		m_size_Factor;
		
		const bool faceIsTaken( const archBodypart& archbodypart, const unsigned int face ) const;
		const bool findOpenFace( unsigned int& bodypart, unsigned int& face ) const;
		
};
#endif
