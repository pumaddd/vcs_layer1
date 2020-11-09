#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Gr-gsm Livemon
# Author: Piotr Krysik
# Description: Interactive monitor of a single C0 channel with analysis performed by Wireshark (command to run wireshark: sudo wireshark -k -f udp -Y gsmtap -i lo)
# Generated: Mon Dec  2 17:15:11 2019
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

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.filter import pfb
from gnuradio.qtgui import Range, RangeWidget
from grgsm import arfcn
from math import pi
from optparse import OptionParser
import grgsm
import math
import pmt
import sys


class grgsm_livemon(gr.top_block, Qt.QWidget):

    def __init__(self, args="", collector="localhost", collectorport="4829", fc=1855.1e6, gain=30, osr=4, ppm=1.15814, samp_rate=4e6, serverport="4829"):
        gr.top_block.__init__(self, "Gr-gsm Livemon")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Gr-gsm Livemon")
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "grgsm_livemon")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Parameters
        ##################################################
        self.args = args
        self.collector = collector
        self.collectorport = collectorport
        self.fc = fc
        self.gain = gain
        self.osr = osr
        self.ppm = ppm
        self.samp_rate = samp_rate
        self.serverport = serverport

        ##################################################
        # Variables
        ##################################################
        self.gsm_symb_rate = gsm_symb_rate = 1625000.0/6.0
        self.samp_rate_out = samp_rate_out = osr*gsm_symb_rate
        self.ppm_slider = ppm_slider = ppm
        self.gain_slider = gain_slider = gain
        self.fc_slider = fc_slider = 1848e6

        ##################################################
        # Blocks
        ##################################################
        self._ppm_slider_range = Range(-150, 150, 0.1, ppm, 100)
        self._ppm_slider_win = RangeWidget(self._ppm_slider_range, self.set_ppm_slider, "PPM Offset", "counter", float)
        self.top_layout.addWidget(self._ppm_slider_win)
        self.pfb_channelizer_ccf_0 = pfb.channelizer_ccf(
        	  20,
        	  (),
        	  2,
        	  100)
        self.pfb_channelizer_ccf_0.set_channel_map(([]))
        self.pfb_channelizer_ccf_0.declare_sample_delay(0)
        	
        self.gsm_receiver_0 = grgsm.receiver(4, ([726]), ([]), fc, 0, False)
        self.gsm_message_printer_0 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_input_0 = grgsm.gsm_input(
            ppm=0,
            osr=osr,
            fc=fc,
            samp_rate_in=200e3*2,
        )
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, "1847100000", False)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_bcch_ccch_demapper_0 = grgsm.gsm_bcch_ccch_demapper(
            timeslot_nr=0,
        )
        self._gain_slider_range = Range(0, 100, 0.5, gain, 100)
        self._gain_slider_win = RangeWidget(self._gain_slider_range, self.set_gain_slider, "Gain", "counter", float)
        self.top_layout.addWidget(self._gain_slider_win)
        self._fc_slider_range = Range(800e6, 1990e6, 2e5, 1848e6, 100)
        self._fc_slider_win = RangeWidget(self._fc_slider_range, self.set_fc_slider, "Frequency", "counter_slider", float)
        self.top_layout.addWidget(self._fc_slider_win)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_socket_pdu_0_1 = blocks.socket_pdu("UDP_CLIENT", collector, collectorport, 1500, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_SERVER", "127.0.0.1", serverport, 10000, False)
        self.blocks_rotator_cc_0 = blocks.rotator_cc(-2*pi*100e3/samp_rate)
        self.blocks_null_sink_9 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_8 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_3 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_0_3 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_0_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_0_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_0_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_7 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_6 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_5 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_4 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_3_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_3 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_10 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_bcch_ccch_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.blocks_socket_pdu_0_1, 'pdus'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_0, 'msgs'))    
        self.msg_connect((self.gsm_receiver_0, 'C0'), (self.gsm_bcch_ccch_demapper_0, 'bursts'))    
        self.connect((self.blocks_rotator_cc_0, 0), (self.pfb_channelizer_ccf_0, 0))    
        self.connect((self.blocks_throttle_0, 0), (self.blocks_rotator_cc_0, 0))    
        self.connect((self.gsm_file_source_C0_0, 0), (self.blocks_throttle_0, 0))    
        self.connect((self.gsm_input_0, 0), (self.gsm_receiver_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 9), (self.blocks_null_sink_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 13), (self.blocks_null_sink_1, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 0), (self.blocks_null_sink_10, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 8), (self.blocks_null_sink_2, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 5), (self.blocks_null_sink_3, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 4), (self.blocks_null_sink_3_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 6), (self.blocks_null_sink_4, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 7), (self.blocks_null_sink_5, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 1), (self.blocks_null_sink_6, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 10), (self.blocks_null_sink_7, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 15), (self.blocks_null_sink_7_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 16), (self.blocks_null_sink_7_0_0, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 17), (self.blocks_null_sink_7_0_1, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 18), (self.blocks_null_sink_7_0_2, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 19), (self.blocks_null_sink_7_0_3, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 14), (self.blocks_null_sink_7_1, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 12), (self.blocks_null_sink_7_2, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 11), (self.blocks_null_sink_7_3, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 2), (self.blocks_null_sink_8, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 3), (self.blocks_null_sink_9, 0))    
        self.connect((self.pfb_channelizer_ccf_0, 4), (self.gsm_input_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "grgsm_livemon")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()


    def get_args(self):
        return self.args

    def set_args(self, args):
        self.args = args

    def get_collector(self):
        return self.collector

    def set_collector(self, collector):
        self.collector = collector

    def get_collectorport(self):
        return self.collectorport

    def set_collectorport(self, collectorport):
        self.collectorport = collectorport

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.gsm_input_0.set_fc(self.fc)

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.set_gain_slider(self.gain)

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr
        self.set_samp_rate_out(self.osr*self.gsm_symb_rate)
        self.gsm_input_0.set_osr(self.osr)

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm
        self.set_ppm_slider(self.ppm)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_rotator_cc_0.set_phase_inc(-2*pi*100e3/self.samp_rate)
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)

    def get_serverport(self):
        return self.serverport

    def set_serverport(self, serverport):
        self.serverport = serverport

    def get_gsm_symb_rate(self):
        return self.gsm_symb_rate

    def set_gsm_symb_rate(self, gsm_symb_rate):
        self.gsm_symb_rate = gsm_symb_rate
        self.set_samp_rate_out(self.osr*self.gsm_symb_rate)

    def get_samp_rate_out(self):
        return self.samp_rate_out

    def set_samp_rate_out(self, samp_rate_out):
        self.samp_rate_out = samp_rate_out

    def get_ppm_slider(self):
        return self.ppm_slider

    def set_ppm_slider(self, ppm_slider):
        self.ppm_slider = ppm_slider

    def get_gain_slider(self):
        return self.gain_slider

    def set_gain_slider(self, gain_slider):
        self.gain_slider = gain_slider

    def get_fc_slider(self):
        return self.fc_slider

    def set_fc_slider(self, fc_slider):
        self.fc_slider = fc_slider


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--args", dest="args", type="string", default="",
        help="Set Device Arguments [default=%default]")
    parser.add_option(
        "", "--collector", dest="collector", type="string", default="localhost",
        help="Set IP or DNS name of collector point [default=%default]")
    parser.add_option(
        "", "--collectorport", dest="collectorport", type="string", default="4829",
        help="Set UDP port number of collector [default=%default]")
    parser.add_option(
        "-f", "--fc", dest="fc", type="eng_float", default=eng_notation.num_to_str(1855.1e6),
        help="Set GSM channel's central frequency [default=%default]")
    parser.add_option(
        "-g", "--gain", dest="gain", type="eng_float", default=eng_notation.num_to_str(30),
        help="Set gain [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=4,
        help="Set OverSampling Ratio [default=%default]")
    parser.add_option(
        "-p", "--ppm", dest="ppm", type="eng_float", default=eng_notation.num_to_str(1.15814),
        help="Set ppm [default=%default]")
    parser.add_option(
        "-s", "--samp-rate", dest="samp_rate", type="eng_float", default=eng_notation.num_to_str(4e6),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--serverport", dest="serverport", type="string", default="4829",
        help="Set UDP server listening port [default=%default]")
    return parser


def main(top_block_cls=grgsm_livemon, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(args=options.args, collector=options.collector, collectorport=options.collectorport, fc=options.fc, gain=options.gain, osr=options.osr, ppm=options.ppm, samp_rate=options.samp_rate, serverport=options.serverport)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
