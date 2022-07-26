#ifndef LCVARS_h
#define LCVARS_h

#define NOVIS // no visuals
#ifndef NOVIS
 #define G4VIS_USE
 #define G4UI_USE
#else
 #undef G4VIS_USE
 #undef G4UI_USE
#endif /*NOVIS*/

//#define TRACES // trajectories collection
#ifdef TRACES
 #define TR_VERBOSE 1 // traces info printout
#else
 #define TR_VERBOSE 0
#endif /*TRACES*/

#define NOREFLECTOR // for absorption length measurement. switch off for light collection simulation
//#define SIMPLE_OPTICS // monochromatic photons, singular absorption length from the app input
//#define NOABSORPTION // optical photons absorption switch
//#define NORAYLEIGH // Rayleigh scattering switch for optical photons
//#define NOMIE // Mie scattering switch
#define WORLD_VACUUM // Replace air with galactic vacuum in the simulation

//#define MATERIAL_VACUUM // Replace CsI with galactic vacuum in the simulation

#define PHOTON_COUNTER // a sensitive part of phyWorld to track escaping photons

#define COLL_GAMMA // collimated gamma instead of ideal (0,1,0)

#define NEW_GEOMETRY // New Detector using another geometry
#endif /*LCVARS_h*/
