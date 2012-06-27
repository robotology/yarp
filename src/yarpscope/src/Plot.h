/*
 *  This file is part of gPortScope
 *
 *  Copyright (C) 2012 Daniele E. Domenichelli <daniele.domenichelli@iit.it>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef GPORTSCOPE_PLOT_H
#define GPORTSCOPE_PLOT_H

#include <gtkdataboxmm/points.h>

namespace GPortScope {

class Plot : public GtkDataboxMM::Points
{
public:
    Plot(std::string inputPortName, unsigned int index);
    virtual ~Plot();

private:
    class Private;
    Private * const mPriv;
};

} // namespace GPortScope
#endif
