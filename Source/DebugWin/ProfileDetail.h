/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __profiledetail_h
#define __profiledetail_h

#include<vector>
#include<algorithm>


class ProfileDetail
{
public:
        int _start, _end;
        AnsiString _tag;

        static const int waiting = 0;
        static const int counting = 1;
        static const int done = 2;

        ProfileDetail(int start, int end) {
                _start = start;
                _end = end;
                Reset();
        }

        void Reset()
        {
                _ts = 0;
                _max = 0;
                _state = waiting;
                _history.clear();
        }

        int Update(int addr, int t) {
                if (addr == _start && _state == waiting) {
                         _ts = 0;
                         _max = 0;
                         _state = counting;
                }

                if (_state == counting) {
                        _ts += t;
                        if (_ts > _max)
                                _max = _ts;
                }

                if (addr == _end && _state == counting) {
                        _state = waiting;
                        _history.push_back(_max);
                }

                return _state;
        }

        int Min() {
                if (_history.empty()) return INT_MAX;
                return *std::min_element(_history.begin(), _history.end());
        }

        int Max() {
                if (_history.empty()) return INT_MIN;
                return *std::max_element(_history.begin(), _history.end());
        }

        int SampleCount()
         {
                int size = _history.size();
                return size;
        }

        int Sample(size_t idx) {
                return idx < _history.size() ? _history[idx] : 0;
        }

private:
        int _ts, _max;
        int _state;

        std::vector<int> _history;
};

#endif
