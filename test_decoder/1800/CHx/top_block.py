#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# Generated: Thu May 14 15:26:45 2020
##################################################
import threading

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
from gnuradio import wxgui
from gnuradio.eng_option import eng_option
from gnuradio.fft import window
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from gnuradio.wxgui import fftsink2
from grc_gnuradio import wxgui as grc_wxgui
from optparse import OptionParser
import grgsm
import math
import pmt
import wx


class top_block(grc_wxgui.top_block_gui):

    def __init__(self, b=5e6, base_port="6700", fc=[1845.2e6, 1850.2e6], fn=0, hsn=60, id_start=1315, ma=[724,727], maio=0, n="724_64707_11413", nstream=2, osr=2, s=5e6, sample_start=153548353, tseq=3, tslot=2):
        grc_wxgui.top_block_gui.__init__(self, title="Top Block")

        self._lock = threading.RLock()

        ##################################################
        # Parameters
        ##################################################
        self.b = b
        self.base_port = base_port
        self.fc = fc
        self.fn = fn
        self.hsn = hsn
        self.id_start = id_start
        self.ma = ma
        self.maio = maio
        self.n = n
        self.nstream = nstream
        self.osr = osr
        self.s = s
        self.sample_start = sample_start
        self.tseq = tseq
        self.tslot = tslot

        ##################################################
        # Variables
        ##################################################
        self.gsm_symb_rate = gsm_symb_rate = 1625000.0/6.0
        self.samp_rate_out = samp_rate_out = gsm_symb_rate*osr

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
        	sample_rate=samp_rate_out,
        	fft_size=1024,
        	fft_rate=15,
        	average=False,
        	avg_alpha=None,
        	title="FFT Plot",
        	peak_hold=False,
        )
        self.Add(self.wxgui_fftsink2_0.win)
        self.pfb_decimator_ccf_0_0 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_BLACKMAN,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0_0.declare_sample_delay(0)
        	
        self.pfb_decimator_ccf_0 = pfb.decimator_ccf(
        	  5,
        	  (firdes.low_pass(1, s,125e3,5e3,firdes.WIN_BLACKMAN,6.76)),
        	  0,
        	  100,
                  True,
                  True)
        self.pfb_decimator_ccf_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*1),
                  taps=None,
        	  flt_size=32)
        self.pfb_arb_resampler_xxx_0_0.declare_sample_delay(0)
        	
        self.pfb_arb_resampler_xxx_0 = pfb.arb_resampler_ccf(
        	  1625000*2/(6e6*1),
                  taps=None,
        	  flt_size=32)
        self.pfb_arb_resampler_xxx_0.declare_sample_delay(0)
        	
        self.gsm_uplink_downlink_splitter_0 = grgsm.uplink_downlink_splitter()
        self.gsm_super_block_0 = grgsm.super_block(osr, (ma), maio, hsn, tseq, s, n, id_start)
        self.gsm_sdcch8_demapper_0 = grgsm.gsm_sdcch8_demapper(
            timeslot_nr=2,
        )
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_hopping_frequency_0 = grgsm.hopping_frequency((ma), (fc), maio, hsn, s, s, n, id_start, s, 0)
        self.gsm_file_source_C0_0_1 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1845200000", True)
        self.gsm_file_source_C0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1850200000", True)
        self.gsm_file_source_C0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1850200000", False)
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, sample_start, "1845200000", False)
        self.gsm_decryption_0 = grgsm.decryption(([0x39,0x37,0x8d,0x4a,0xe5,0x48,0xb4,0xcf]), 1)
        self.gsm_cx_channel_hopper_0 = grgsm.cx_channel_hopper(([724,727]), 0, 60)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_burst_file_sink_0 = grgsm.burst_file_sink("/tmp/bursts")
        self.blocks_throttle_0_2_1 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2_0_0 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2_0 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_throttle_0_2 = blocks.throttle(gr.sizeof_gr_complex*1, s,True)
        self.blocks_socket_pdu_0_1 = blocks.socket_pdu("UDP_CLIENT", "127.0.0.1", "4729", 1500, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_SERVER", "127.0.0.1", "4729", 10000, False)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.blocks_socket_pdu_0_1, 'pdus'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_cx_channel_hopper_0, 'bursts'), (self.gsm_burst_file_sink_0, 'in'))    
        self.msg_connect((self.gsm_cx_channel_hopper_0, 'bursts'), (self.gsm_sdcch8_demapper_0, 'bursts'))    
        self.msg_connect((self.gsm_decryption_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_sdcch8_demapper_0, 'bursts'), (self.gsm_decryption_0, 'bursts'))    
        self.msg_connect((self.gsm_super_block_0, 'CX'), (self.gsm_uplink_downlink_splitter_0, 'in'))    
        self.msg_connect((self.gsm_uplink_downlink_splitter_0, 'downlink'), (self.gsm_cx_channel_hopper_0, 'CX'))    
        self.connect((self.blocks_throttle_0_2, 0), (self.gsm_hopping_frequency_0, 0))    
        self.connect((self.blocks_throttle_0_2_0, 0), (self.gsm_hopping_frequency_0, 1))    
        self.connect((self.blocks_throttle_0_2_0_0, 0), (self.gsm_hopping_frequency_0, 3))    
        self.connect((self.blocks_throttle_0_2_1, 0), (self.gsm_hopping_frequency_0, 2))    
        self.connect((self.gsm_file_source_C0_0, 0), (self.blocks_throttle_0_2, 0))    
        self.connect((self.gsm_file_source_C0_0_0, 0), (self.blocks_throttle_0_2_0, 0))    
        self.connect((self.gsm_file_source_C0_0_0_0, 0), (self.blocks_throttle_0_2_0_0, 0))    
        self.connect((self.gsm_file_source_C0_0_1, 0), (self.blocks_throttle_0_2_1, 0))    
        self.connect((self.gsm_hopping_frequency_0, 1), (self.pfb_decimator_ccf_0, 0))    
        self.connect((self.gsm_hopping_frequency_0, 0), (self.pfb_decimator_ccf_0_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0, 0), (self.gsm_super_block_0, 1))    
        self.connect((self.pfb_arb_resampler_xxx_0_0, 0), (self.gsm_super_block_0, 0))    
        self.connect((self.pfb_arb_resampler_xxx_0_0, 0), (self.wxgui_fftsink2_0, 0))    
        self.connect((self.pfb_decimator_ccf_0, 0), (self.pfb_arb_resampler_xxx_0, 0))    
        self.connect((self.pfb_decimator_ccf_0_0, 0), (self.pfb_arb_resampler_xxx_0_0, 0))    

    def get_b(self):
        return self.b

    def set_b(self, b):
        with self._lock:
            self.b = b

    def get_base_port(self):
        return self.base_port

    def set_base_port(self, base_port):
        with self._lock:
            self.base_port = base_port

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        with self._lock:
            self.fc = fc

    def get_fn(self):
        return self.fn

    def set_fn(self, fn):
        with self._lock:
            self.fn = fn

    def get_hsn(self):
        return self.hsn

    def set_hsn(self, hsn):
        with self._lock:
            self.hsn = hsn

    def get_id_start(self):
        return self.id_start

    def set_id_start(self, id_start):
        with self._lock:
            self.id_start = id_start

    def get_ma(self):
        return self.ma

    def set_ma(self, ma):
        with self._lock:
            self.ma = ma

    def get_maio(self):
        return self.maio

    def set_maio(self, maio):
        with self._lock:
            self.maio = maio

    def get_n(self):
        return self.n

    def set_n(self, n):
        with self._lock:
            self.n = n

    def get_nstream(self):
        return self.nstream

    def set_nstream(self, nstream):
        with self._lock:
            self.nstream = nstream

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        with self._lock:
            self.osr = osr
            self.set_samp_rate_out(self.gsm_symb_rate*self.osr)

    def get_s(self):
        return self.s

    def set_s(self, s):
        with self._lock:
            self.s = s
            self.blocks_throttle_0_2.set_sample_rate(self.s)
            self.blocks_throttle_0_2_0.set_sample_rate(self.s)
            self.blocks_throttle_0_2_0_0.set_sample_rate(self.s)
            self.blocks_throttle_0_2_1.set_sample_rate(self.s)
            self.pfb_decimator_ccf_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_BLACKMAN,6.76)))
            self.pfb_decimator_ccf_0_0.set_taps((firdes.low_pass(1, self.s,125e3,5e3,firdes.WIN_BLACKMAN,6.76)))

    def get_sample_start(self):
        return self.sample_start

    def set_sample_start(self, sample_start):
        with self._lock:
            self.sample_start = sample_start

    def get_tseq(self):
        return self.tseq

    def set_tseq(self, tseq):
        with self._lock:
            self.tseq = tseq

    def get_tslot(self):
        return self.tslot

    def set_tslot(self, tslot):
        with self._lock:
            self.tslot = tslot

    def get_gsm_symb_rate(self):
        return self.gsm_symb_rate

    def set_gsm_symb_rate(self, gsm_symb_rate):
        with self._lock:
            self.gsm_symb_rate = gsm_symb_rate
            self.set_samp_rate_out(self.gsm_symb_rate*self.osr)

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        with self._lock:
            self.samp_rate_out = samp_rate_out
            self.wxgui_fftsink2_0.set_sample_rate(self.samp_rate_out)


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--b", dest="b", type="eng_float", default=eng_notation.num_to_str(5e6),
        help="Set Band width [default=%default]")
    parser.add_option(
        "", "--base-port", dest="base_port", type="string", default="6700",
        help="Set 6700 [default=%default]")
    parser.add_option(
        "", "--fn", dest="fn", type="intx", default=0,
        help="Set Frame Number [default=%default]")
    parser.add_option(
        "", "--hsn", dest="hsn", type="intx", default=60,
        help="Set hsn [default=%default]")
    parser.add_option(
        "", "--id-start", dest="id_start", type="intx", default=1315,
        help="Set ID start [default=%default]")
    parser.add_option(
        "", "--maio", dest="maio", type="intx", default=0,
        help="Set maio [default=%default]")
    parser.add_option(
        "", "--n", dest="n", type="string", default="724_64707_11413",
        help="Set Database name [default=%default]")
    parser.add_option(
        "", "--nstream", dest="nstream", type="intx", default=2,
        help="Set Number Stream [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=2,
        help="Set OSR [default=%default]")
    parser.add_option(
        "", "--s", dest="s", type="eng_float", default=eng_notation.num_to_str(5e6),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--sample-start", dest="sample_start", type="intx", default=153548353,
        help="Set Sample Start [default=%default]")
    parser.add_option(
        "", "--tseq", dest="tseq", type="intx", default=3,
        help="Set Traning Sequence [default=%default]")
    parser.add_option(
        "", "--tslot", dest="tslot", type="intx", default=2,
        help="Set Time Slot [default=%default]")
    return parser


def main(top_block_cls=top_block, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    tb = top_block_cls(b=options.b, base_port=options.base_port, fn=options.fn, hsn=options.hsn, id_start=options.id_start, maio=options.maio, n=options.n, nstream=options.nstream, osr=options.osr, s=options.s, sample_start=options.sample_start, tseq=options.tseq, tslot=options.tslot)
    tb.Start(True)
    tb.Wait()


if __name__ == '__main__':
    main()
