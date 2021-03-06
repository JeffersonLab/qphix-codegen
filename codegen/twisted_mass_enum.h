#pragma once

#include <map>
#include <string>
#include <vector>

/**
  Versions of twisted mass.

  Previously, there was a simple `bool` value `twisted_mass` that
  would switch
  twisted mass on or off. Now with the non-degenerate case, there
  are three
  options that need to be represented. Having a second `bool` named
  `non_degenerate` would not suffice as the combination of
  non-degernate
  non-twisted mass does not make sense.
  */
enum class TwistedMassVariant {
  /// Plain Wilson fermions.
  none,
  /// Degenerate, \f$ N_\mathrm f = 2 \f$, Wilson twisted mass
  /// fermions.
  degenerate,
  /// Non-degernate, \f$ N_\mathrm f = 1 + 1 \f$, Wilson twisted
  /// mass
  /// fermions.
  non_degenerate,
};

std::vector<TwistedMassVariant> const
    selected_twisted_mass_variants{TwistedMassVariant::none,
                                   TwistedMassVariant::degenerate};

/// Prefixes for the generated kernels.
std::map<TwistedMassVariant const, std::string const> const
    twisted_mass_prefixes = {
        {TwistedMassVariant::none, std::string("")},
        {TwistedMassVariant::degenerate, std::string("tm_")},
        {TwistedMassVariant::non_degenerate, std::string("ndtm_")},
};
