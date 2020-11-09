#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Mon Dec  2 17:46:23 2019
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

from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import uhd
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from gnuradio.wxgui import fftsink2
from grc_gnuradio import wxgui as grc_wxgui
from grgsm import arfcn
from math import pi
from optparse import OptionParser
import grgsm
import math
import pmt
import time
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self, f="/dev/sdc6", fc=1849.9e6, fc0=arfcn.arfcn2downlink(726), fc_0=1857.9e6, n="726", osr=2, s=4e6):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")
        _icon_path = "/usr/share/icons/hicolor/32x32/apps/gnuradio-grc.png"
        self.SetIcon(wx.Icon(_icon_path, wx.BITMAP_TYPE_ANY))

        ##################################################
        # Parameters
        ##################################################
        self.f = f
        self.fc = fc
        self.fc0 = fc0
        self.fc_0 = fc_0
        self.n = n
        self.osr = osr
        self.s = s

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_out = samp_rate_out = 1625000*4/6
        self.samp_rate = samp_rate = 4e6

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
        	sample_rate=samp_rate_out/2,
        	fft_size=1024,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title="FFT Plot",
        	peak_hold=False,
        )
        self.Add(self.wxgui_fftsink2_0.win)
        self.uhd_usrp_source_0 = uhd.usrp_source(
        	",".join(("", "")),
        	uhd.stream_args(
        		cpu_format="fc32",
        		channels=range(1),
        	),
        )
        self.uhd_usrp_source_0.set_samp_rate(20e6)
        self.uhd_usrp_source_0.set_center_freq(fc_0, 0)
        self.uhd_usrp_source_0.set_gain(30, 0)
        self.uhd_usrp_source_0.set_bandwidth(22e6, 0)
        self.pfb_decimator_ccf_0 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_HAMMING,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0.declare_sample_delay(0)
        	
        self.pfb_channelizer_ccf_0 = pfb.channelizer_ccf(
        	  5,
        	  (firdes.low_pass(32, 20e6,2e6,5e3,firdes.WIN_HAMMING,6.76)),
        	  1.0,
        	  100)
        self.pfb_channelizer_ccf_0.set_channel_map(([]))
        self.pfb_channelizer_ccf_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*0.8),
                  taps=None,
        	  flt_size=10)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_decodeC0_0 = grgsm.decodeC0(osr, ([arfcn.downlink2arfcn(fc0)]), fc, fc0, s, 0, n, False)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_bcch_ccch_demapper_0 = grgsm.gsm_bcch_ccch_demapper(
            timeslot_nr=0,
        )
        self.blocks_rotator_cc_0 = blocks.rotator_cc(-2*pi*(fc0 - fc)/s)
        self.blocks_null_sink_0_3 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_bcch_ccch_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_decodeC0_0, 'C0'), (self.gsm_bcch_ccch_demapper_0, 'bursts'))    
        self.connect((self.blocks_rotator_cc_0, 0), (self.pfb_decimator_ccf_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.gsm_decodeC0_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.wxgui_fftsink2_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 0), (self.blocks_null_sink_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 2), (self.blocks_null_sink_0_1, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 1), (self.blocks_null_sink_0_2, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 4), (self.blocks_null_sink_0_3, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 3), (self.blocks_rotator_cc_0, 0))    
        self.connect((self.pfb_decimator_ccf_0, 0), (self.pfb_arb_resampler_xxx_0, 0))    
        self.connect((self.uhd_usrp_source_0, 0), (self.pfb_channelizer_ccf_0, 0))    

    def get_f(self):
        return self.f

    def set_f(self, f):
        self.f = f

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_fc0(self):
        return self.fc0

    def set_fc0(self, fc0):
        self.fc0 = fc0
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)

    def get_fc_0(self):
        return self.fc_0

    def set_fc_0(self, fc_0):
        self.fc_0 = fc_0
        self.uhd_usrp_source_0.set_center_freq(self.fc_0, 0)

    def get_n(self):
        return self.n

    def set_n(self, n):
        self.n = n

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr

    def get_s(self):
        return self.s

    def set_s(self, s):
        self.s = s
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*(self.fc0 - self.fc)/self.s)
        self.pfb_decimator_ccf_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_HAMMING,6.76)))

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        self.samp_rate_out = samp_rate_out
        self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate_out/2)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.uhd_usrp_source_0.set_bandwidth(self.samp_rate, 1)


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--f", dest="f", type="string", default="/dev/sdc6",
        help="Set CH0 file path [default=%default]")
    parser.add_option(
        "", "--fc", dest="fc", type="eng_float", default=eng_notation.num_to_str(1849.9e6),
        help="Set frequency center [default=%default]")
    parser.add_option(
        "", "--fc0", dest="fc0", type="eng_float", default=eng_notation.num_to_str(arfcn.arfcn2downlink(726)),
        help="Set frequency CH0 [default=%default]")
    parser.add_option(
        "", "--fc-0", dest="fc_0", type="eng_float", default=eng_notation.num_to_str(1857.9e6),
        help="Set frequency center capture [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="726",
        help="Set Name of database [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=2,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(4e6),
        help="Set samp_rate [default=%default]")
    return parser


def main(top_block_cls=top_block, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(f=options.f, fc=options.fc, fc0=options.fc0, fc_0=options.fc_0, n=options.n, osr=options.osr, s=options.s)
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
