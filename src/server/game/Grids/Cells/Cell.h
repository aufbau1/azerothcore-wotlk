/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ACORE_CELL_H
#define ACORE_CELL_H

#include "GridDefines.h"
#include "TypeContainerVisitor.h"

class Map;
class WorldObject;

struct CellArea
{
    CellArea() = default;
    CellArea(CellCoord low, CellCoord high) : low_bound(low), high_bound(high) {}

    bool operator!() const { return low_bound == high_bound; }

    void ResizeBorders(CellCoord& begin_cell, CellCoord& end_cell) const
    {
        begin_cell = low_bound;
        end_cell = high_bound;
    }

    CellCoord low_bound;
    CellCoord high_bound;
};

struct Cell
{
    Cell() { data.All = 0; }
    Cell(Cell const& cell) { data.All = cell.data.All; }
    explicit Cell(CellCoord const& p);
    explicit Cell(float x, float y);

    void Compute(uint32& x, uint32& y) const
    {
        x = data.Part.grid_x * MAX_NUMBER_OF_CELLS + data.Part.cell_x;
        y = data.Part.grid_y * MAX_NUMBER_OF_CELLS + data.Part.cell_y;
    }

    [[nodiscard]] bool DiffCell(const Cell& cell) const
    {
        return(data.Part.cell_x != cell.data.Part.cell_x ||
               data.Part.cell_y != cell.data.Part.cell_y);
    }

    [[nodiscard]] bool DiffGrid(const Cell& cell) const
    {
        return(data.Part.grid_x != cell.data.Part.grid_x ||
               data.Part.grid_y != cell.data.Part.grid_y);
    }

    [[nodiscard]] uint32 CellX() const { return data.Part.cell_x; }
    [[nodiscard]] uint32 CellY() const { return data.Part.cell_y; }
    [[nodiscard]] uint32 GridX() const { return data.Part.grid_x; }
    [[nodiscard]] uint32 GridY() const { return data.Part.grid_y; }

    [[nodiscard]] CellCoord GetCellCoord() const
    {
        return CellCoord(
                   data.Part.grid_x * MAX_NUMBER_OF_CELLS + data.Part.cell_x,
                   data.Part.grid_y * MAX_NUMBER_OF_CELLS + data.Part.cell_y);
    }

    Cell& operator=(Cell const& cell)
    {
        this->data.All = cell.data.All;
        return *this;
    }

    bool operator == (Cell const& cell) const { return (data.All == cell.data.All); }
    bool operator != (Cell const& cell) const { return !operator == (cell); }
    union
    {
        struct
        {
            unsigned grid_x : 8;
            unsigned grid_y : 8;
            unsigned cell_x : 8;
            unsigned cell_y : 8;
        } Part;
        uint32 All;
    } data;

    template<class T, class CONTAINER> void Visit(CellCoord const&, TypeContainerVisitor<T, CONTAINER>& visitor, Map&, WorldObject const& obj, float radius) const;
    template<class T, class CONTAINER> void Visit(CellCoord const&, TypeContainerVisitor<T, CONTAINER>& visitor, Map&, float x, float y, float radius) const;

    static CellArea CalculateCellArea(float x, float y, float radius);

    template<class T> static void VisitGridObjects(WorldObject const* obj, T& visitor, float radius);
    template<class T> static void VisitWorldObjects(WorldObject const* obj, T& visitor, float radius);
    template<class T> static void VisitAllObjects(WorldObject const* obj, T& visitor, float radius);

    template<class T> static void VisitGridObjects(float x, float y, Map* map, T& visitor, float radius);
    template<class T> static void VisitWorldObjects(float x, float y, Map* map, T& visitor, float radius);
    template<class T> static void VisitAllObjects(float x, float y, Map* map, T& visitor, float radius);

private:
    template<class T, class CONTAINER> void VisitCircle(TypeContainerVisitor<T, CONTAINER>&, Map&, CellCoord const&, CellCoord const&) const;
};

#endif
