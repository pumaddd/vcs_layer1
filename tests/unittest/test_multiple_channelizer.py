#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Test Multiple Channelizer
# Generated: Sun Apr 12 02:07:38 2020
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

import os
import sys
sys.path.append(os.environ.get('GRC_HIER_PATH', os.path.expanduser('~/.grc_gnuradio')))

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.wxgui import fftsink2
from grc_gnuradio import wxgui as grc_wxgui
from grgsm import arfcn
from grgsm import grd_config
from grgsm.multichannel_file_source import multi_channelize_layer  # grc-generated hier_block
from optparse import OptionParser
import grgsm
import wx


class test_multiple_channelizer(grc_wxgui.top_block_gui):

    def __init__(self):
        grc_wxgui.top_block_gui.__init__(self, title="Test Multiple Channelizer")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Variables
        ##################################################
        self.start_900 = start_900 = 947500000
        self.samp_rate = samp_rate = 3e6
        self.bandwidth_900 = bandwidth_900 = 2000000.0

        ##################################################
        # Blocks
        ##################################################
        self.wxgui_fftsink2_0 = fftsink2.fft_sink_c(
        	self.GetWin(),
        	baseband_freq=0,
        	y_per_div=10,
        	y_divs=10,
        	ref_level=0,
        	ref_scale=2.0,
        	sample_rate=samp_rate/15,
        	fft_size=1024,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title="FFT Plot 1",
        	peak_hold=False,
        )
        self.Add(self.wxgui_fftsink2_0.win)
        self.multichannel_file_source_0 = multi_channelize_layer(
              ([start_900,start_900-2*bandwidth_900,start_900-bandwidth_900, start_900+2*bandwidth_900,start_900+bandwidth_900]),
              ([bandwidth_900,bandwidth_900,bandwidth_900,bandwidth_900,bandwidth_900]),
              ([47,53,55]),
              )
          
        self.gsm_file_source_C0_0_0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900+bandwidth_900), False)
        self.gsm_file_source_C0_0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900+2*bandwidth_900), False)
        self.gsm_file_source_C0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900-bandwidth_900), False)
        self.gsm_file_source_C0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900-2*bandwidth_900), False)
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900), False)
        self.blocks_null_sink_0_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.connect((self.gsm_file_source_C0_0, 0), (self.multichannel_file_source_0, 0))    
        self.connect((self.gsm_file_source_C0_0_0, 0), (self.multichannel_file_source_0, 1))    
        self.connect((self.gsm_file_source_C0_0_0_0, 0), (self.multichannel_file_source_0, 2))    
        self.connect((self.gsm_file_source_C0_0_0_0_0, 0), (self.multichannel_file_source_0, 3))    
        self.connect((self.gsm_file_source_C0_0_0_0_0_0, 0), (self.multichannel_file_source_0, 4))    
        self.connect((self.multichannel_file_source_0, 2), (self.blocks_null_sink_0, 0))    
        self.connect((self.multichannel_file_source_0, 0), (self.blocks_null_sink_0_0, 0))    
        self.connect((self.multichannel_file_source_0, 1), (self.blocks_null_sink_0_0_0, 0))    
        self.connect((self.multichannel_file_source_0, 0), (self.wxgui_fftsink2_0, 0))    

    def get_start_900(self):
        return self.start_900

    def set_start_900(self, start_900):
        self.start_900 = start_900

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate/15)

    def get_bandwidth_900(self):
        return self.bandwidth_900

    def set_bandwidth_900(self, bandwidth_900):
        self.bandwidth_900 = bandwidth_900


def main(top_block_cls=test_multiple_channelizer, options=None):

    tb = top_block_cls()
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
