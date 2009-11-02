/* Generated by Together */

#include "TaskLeg.hpp"
#include "Math/Earth.hpp"
#include "OrderedTaskPoint.hpp"
#include <assert.h>
#include <algorithm>


TaskLeg::TaskLeg(OrderedTaskPoint &_destination):
    vector_travelled(0.0,0.0),
    vector_remaining(0.0,0.0),
    vector_planned(0.0,0.0),
    destination(_destination)
{

}

const OrderedTaskPoint* 
TaskLeg::origin() const
{
  return destination.get_previous();
}

OrderedTaskPoint* 
TaskLeg::next() const
{
  return destination.get_next();
}

//////////////////////////////////////////////

GeoVector 
TaskLeg::leg_vector_planned() const
{
  if (!origin()) {
    return 0.0;
  } else {
    return memo_planned.calc(origin()->get_reference_remaining(), 
                             destination.get_reference_remaining());
  }
}


GeoVector 
TaskLeg::leg_vector_remaining(const GEOPOINT &ref) const
{
  if (!origin()) {
    return GeoVector(0.0, ref.bearing(destination.get_reference_remaining()));
  }
  switch (destination.getActiveState()) {
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg totally included
    return memo_remaining.calc(origin()->get_reference_remaining(), 
                               destination.get_reference_remaining());
    break;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    return memo_remaining.calc(ref, 
                               destination.get_reference_remaining());
    break;
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg not included
  default:
    assert(1); // error!
    return 0.0;
  };
}


GeoVector
TaskLeg::leg_vector_travelled(const GEOPOINT &ref) const
{
  if (!origin()) {
    return GeoVector(0.0, ref.bearing(destination.get_reference_remaining()));
  }
  switch (destination.getActiveState()) {
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg totally included
    return memo_travelled.calc(origin()->get_reference_travelled(), 
                               destination.get_reference_travelled());
    break;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    if (destination.has_entered()) {
      return memo_travelled.calc(origin()->get_reference_travelled(), 
                                 destination.get_reference_travelled());
    } else {
      return memo_travelled.calc(origin()->get_reference_travelled(), 
                                 ref);
    }
    break;
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg may be partially included
    if (origin()->has_entered()) {
      return memo_travelled.calc(origin()->get_reference_travelled(), 
                                 ref);
    }
  default:
    return 0.0;
  };
}


double TaskLeg::leg_distance_scored(const GEOPOINT &ref) const
{
  if (!origin()) {
    return 0.0;
  }

  switch (destination.getActiveState()) {
  case OrderedTaskPoint::BEFORE_ACTIVE:
    // this leg totally included
    return 
      std::max(0.0,
               origin()->get_reference_scored().distance(
                 destination.get_reference_scored())
               -origin()->score_adjustment()-destination.score_adjustment());
    break;
  case OrderedTaskPoint::CURRENT_ACTIVE:
    // this leg partially included
    if (destination.has_entered()) {
      std::max(0.0,
               origin()->get_reference_scored().distance( 
                 destination.get_reference_scored())
               -origin()->score_adjustment()-destination.score_adjustment());
    } else {
      return 
        std::max(0.0,
                 ::ProjectedDistance(origin()->get_reference_scored(), 
                                     destination.get_reference_scored(),
                                     ref)
                 -origin()->score_adjustment()-destination.score_adjustment());
    }
    break;
  case OrderedTaskPoint::AFTER_ACTIVE:
    // this leg may be partially included
    if (origin()->has_entered()) {
      return std::max(0.0,
                      memo_travelled.calc(origin()->get_reference_scored(), 
                                          ref).Distance
                      -origin()->score_adjustment());
    }
  default:
    return 0.0;
    break;
  };
  return 0.0;
}


double TaskLeg::leg_distance_nominal() const
{
  if (!origin()) {
    return 0.0; 
  } else {
    return memo_nominal.Distance(origin()->get_reference_nominal(), 
                                 destination.get_reference_nominal());
  }
}


double TaskLeg::leg_distance_max() const
{
  if (!origin()) {
    return 0.0; 
  } else {
    return memo_max.Distance(origin()->getMaxLocation(), 
                             destination.getMaxLocation());
  }
}

double TaskLeg::leg_distance_min() const
{
  if (!origin()) {
    return 0.0; 
  } else {
    return memo_min.Distance(origin()->getMinLocation(), 
                             destination.getMinLocation());
  }
}

/////////////////

double 
TaskLeg::scan_distance_travelled(const GEOPOINT &ref) 
{
  vector_travelled = leg_vector_travelled(ref);
  return vector_travelled.Distance 
    +(next()? next()->scan_distance_travelled(ref):0);
}


double 
TaskLeg::scan_distance_remaining(const GEOPOINT &ref) 
{
  vector_remaining = leg_vector_remaining(ref);
  return vector_remaining.Distance 
    +(next()? next()->scan_distance_remaining(ref):0);
}


double 
TaskLeg::scan_distance_planned() 
{
  vector_planned = leg_vector_planned();
  return vector_planned.Distance 
    +(next()? next()->scan_distance_planned():0);
}


double 
TaskLeg::scan_distance_max() 
{
  return leg_distance_max()
    +(next()? next()->scan_distance_max():0);
}


double 
TaskLeg::scan_distance_min() 
{
  return leg_distance_min()
    +(next()? next()->scan_distance_min():0);
}


double 
TaskLeg::scan_distance_nominal() 
{
  return leg_distance_nominal()
    +(next()? next()->scan_distance_nominal():0);
}

double 
TaskLeg::scan_distance_scored(const GEOPOINT &ref) 
{
  return leg_distance_scored(ref)
    +(next()? next()->scan_distance_scored(ref):0);
}
