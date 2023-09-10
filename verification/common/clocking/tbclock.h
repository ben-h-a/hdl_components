////////////////////////////////////////////////////////////////////////////////
//
// Filename: 	tbclock.h
//
// Project:	VideoZip, a ZipCPU SoC supporting video functionality
//
// Purpose:	
//
// Creator:	Dan Gisselquist, Ph.D.
//		Gisselquist Technology, LLC
//
////////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2015-2017, Gisselquist Technology, LLC
//
// This program is free software (firmware): you can redistribute it and/or
// modify it under the terms of  the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTIBILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program.  (It's in the $(ROOT)/doc directory.  Run make with no
// target there if the PDF file isn't present.)  If not, see
// <http://www.gnu.org/licenses/> for a copy.
//
// License:	GPL, v3, as defined and found on www.gnu.org,
//		http://www.gnu.org/licenses/gpl.html
//
//
////////////////////////////////////////////////////////////////////////////////
//
//
#ifndef	TBCLOCK_H
#define	TBCLOCK_H


class	TBCLOCK	{
	unsigned long	m_increment_ps, m_now_ps, m_last_edge_ps;

public:
	TBCLOCK(void) {
		m_increment_ps = 10000; // 10 ns;

		m_now_ps = m_increment_ps+1;
		m_last_edge_ps = m_increment_ps;
	}

	TBCLOCK(unsigned long increment_ps) {
		init(increment_ps);
	}

	void	init(unsigned long increment_ps) {
		set_interval_ps(increment_ps);

		// Start with the clock low, waiting on a positive edge
		m_now_ps = m_increment_ps+1;
		m_last_edge_ps = m_increment_ps;
	}

	unsigned long	time_to_tick(void) {
		unsigned long	ul;
		if (m_last_edge_ps > m_now_ps) {
			// Should never happen
			ul = m_last_edge_ps - m_now_ps;
			ul /= m_increment_ps;
			ul = m_now_ps + ul * m_increment_ps;
		} else if (m_last_edge_ps == m_now_ps) {
			ul = m_increment_ps;
		} else if (m_last_edge_ps + m_increment_ps == m_now_ps) {
			ul = m_increment_ps;
		} else if (m_last_edge_ps + m_increment_ps > m_now_ps) {
			ul = m_last_edge_ps + m_increment_ps - m_now_ps;
		} else // if (m_last_edge + m_interval_ps > m_now) {
			ul = (m_last_edge_ps + 2*m_increment_ps - m_now_ps);

		return ul;
	}

	void	set_interval_ps(unsigned long interval_ps) {
		// Divide the clocks interval by two, so we can have a
		// period for raising the clock, and another for lowering
		// the clock.
		m_increment_ps = (interval_ps>>1)&-2l;
		assert(m_increment_ps > 0);
	}

	int	advance(unsigned long itime)  {
		int	clk = 0;
		m_now_ps += itime;
		if (m_now_ps >= m_last_edge_ps + 2*m_increment_ps) {
			m_last_edge_ps += 2*m_increment_ps;
			clk = 1;
		} else if (m_now_ps >= m_last_edge_ps + m_increment_ps)
			clk = 0;
		else
			clk = 1;
		return clk;
	}

	bool	rising_edge(void) {
		if (m_now_ps == m_last_edge_ps) {
			return true;
		} return false;
	}

	bool	falling_edge(void) {
		if (m_now_ps == m_last_edge_ps + m_increment_ps) {
			return true;
		} return false;
	}
};


bool tick_tbclock(TBCLOCK * clocks, int num_clks, void *) {
    uint32_t mintime = UINT32_MAX;

    for(int i=0; i<num_clks; i++){
        if(clocks[i].time_to_tick() < mintime){
            mintime = clocks[i].time_to_tick();
        }
    }
    
    assert(mintime > 1);

    m_core->eval();
    if (m_trace) m_trace->dump(m_time_ps+1);

    m_core->i_clk = m_clk.advance(mintime);
    m_core->i_hdmi_out_clk = m_hdmi_out_clk.advance(mintime);
    m_core->i_hdmi_in_clk = m_hdmi_in_clk.advance(mintime);
    m_core->i_hdmi_in_hsclk = m_hdmi_in_hsclk.advance(mintime);
    m_core->i_clk_200mhz = m_clk_200mhz.advance(mintime);

    m_time_ps += mintime;

    m_core->eval();
    if (m_trace) {
        m_trace->dump(m_time_ps+1);
        m_trace->flush();
    }

    if (m_clk.falling_edge()) {
        m_changed = true;
        sim_clk_tick();
    }
    if (m_hdmi_out_clk.falling_edge()) {
        m_changed = true;
        sim_hdmi_out_clk_tick();
    }
    if (m_hdmi_in_clk.falling_edge()) {
        m_changed = true;
        sim_hdmi_in_clk_tick();
    }
    if (m_hdmi_in_hsclk.falling_edge()) {
        m_changed = true;
        sim_hdmi_in_hsclk_tick();
    }
    if (m_clk_200mhz.falling_edge()) {
        m_changed = true;
        sim_clk_200mhz_tick();
    }
}

#endif