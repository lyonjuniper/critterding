#ifndef WORLDRACE_H
#define WORLDRACE_H

#include "../entities/worldb.h"

class WorldRace : public WorldB {

	public:
		WorldRace(  boost::shared_ptr<BeGraphicsSystem> system, boost::shared_ptr<Textverbosemessage> textverbosemessage );
		~WorldRace();

		void		process(const BeTimer& timer);
		void		init();
		void		insertCritter();
		void		loadAllCritters();
	private:
		void		makeFloor();
		void		insRandomCritter(int nr);
		void		insMutatedCritter(CritterB& other, int nr, unsigned int id, bool mutateBrain, bool mutateBody);
		void		insFood(int nr);
		
		float		critterspacing;

		bool		haveWinner;

		unsigned int	framecounter;
		
		unsigned int	testcounter;
};

#endif
