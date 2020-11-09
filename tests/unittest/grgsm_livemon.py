#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Gr-gsm Livemon
# Author: Piotr Krysik
# Description: Interactive monitor of a single C0 channel with analysis performed by Wireshark (command to run wireshark: sudo wireshark -k -f udp -Y gsmtap -i lo)
# Generated: Sun Apr 12 13:10:36 2020
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

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio import qtgui
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from gnuradio.qtgui import Range, RangeWidget
from grgsm import arfcn
from grgsm import grd_config
from grgsm.multichannel_file_source import multi_channelize_layer  # grc-generated hier_block
from math import pi
from optparse import OptionParser
import grgsm
import pmt
import sip


class grgsm_livemon(gr.top_block, Qt.QWidget):

    def __init__(self, args="", collector="localhost", collectorport="4729", gain=30, osr=2, ppm=0, samp_rate=2000000.052982, serverport="4729", shiftoff=400e3):
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
        self.gain = gain
        self.osr = osr
        self.ppm = ppm
        self.samp_rate = samp_rate
        self.serverport = serverport
        self.shiftoff = shiftoff

        ##################################################
        # Variables
        ##################################################
        self.c0 = c0 = 53
        self.fc = fc = arfcn.arfcn2downlink(c0)
        self.start_900 = start_900 = 947500000
        self.samp_rate_out = samp_rate_out = 1.083333333e6*osr/4
        self.ppm_slider = ppm_slider = ppm
        self.gain_slider = gain_slider = gain
        self.fc_slider = fc_slider = fc
        self.bandwidth_900 = bandwidth_900 = 2000000.0

        ##################################################
        # Blocks
        ##################################################
        self._fc_slider_range = Range(800e6, 1990e6, 2e5, fc, 100)
        self._fc_slider_win = RangeWidget(self._fc_slider_range, self.set_fc_slider, "Frequency", "counter_slider", float)
        self.top_layout.addWidget(self._fc_slider_win)
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
        	1024, #size
        	firdes.WIN_BLACKMAN_hARRIS, #wintype
        	fc_slider, #fc
        	200e3, #bw
        	"", #name
        	1 #number of inputs
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)
        
        if not True:
          self.qtgui_freq_sink_x_0.disable_legend()
        
        if "complex" == "float" or "complex" == "msg_float":
          self.qtgui_freq_sink_x_0.set_plot_pos_half(not True)
        
        labels = ["", "", "", "", "",
                  "", "", "", "", ""]
        widths = [1, 1, 1, 1, 1,
                  1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
                  "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
                  1.0, 1.0, 1.0, 1.0, 1.0]
        for i in xrange(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])
        
        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self._ppm_slider_range = Range(-150, 150, 0.1, ppm, 100)
        self._ppm_slider_win = RangeWidget(self._ppm_slider_range, self.set_ppm_slider, "PPM Offset", "counter", float)
        self.top_layout.addWidget(self._ppm_slider_win)
        self.multichannel_file_source_0 = multi_channelize_layer(
              ([start_900,start_900-2*bandwidth_900,start_900-bandwidth_900, start_900+2*bandwidth_900,start_900+bandwidth_900]),
              ([bandwidth_900,bandwidth_900,bandwidth_900,bandwidth_900,bandwidth_900]),
              ([54,55,56]),
              )
          
        self.gsm_receiver_0 = grgsm.receiver(osr, ([c0]), ([]), int(fc), 0, False)
        self.gsm_message_printer_1 = grgsm.message_printer(pmt.intern(""), False,
            False, False)
        self.gsm_input_1 = grgsm.gsm_input(
            ppm=ppm,
            osr=osr,
            fc=fc,
            samp_rate_in=200000,
        )
        self.gsm_file_source_C0_0_0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900+bandwidth_900), False)
        self.gsm_file_source_C0_0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900+2*bandwidth_900), False)
        self.gsm_file_source_C0_0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900-bandwidth_900), False)
        self.gsm_file_source_C0_0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900-2*bandwidth_900), False)
        self.gsm_file_source_C0_0 = grgsm.file_source_C0(gr.sizeof_gr_complex*1, 0, str(start_900), False)
        self.gsm_control_channels_decoder_0 = grgsm.control_channels_decoder()
        self.gsm_bcch_ccch_demapper_0 = grgsm.gsm_bcch_ccch_demapper(
            timeslot_nr=0,
        )
        self._gain_slider_range = Range(0, 100, 0.5, gain, 100)
        self._gain_slider_win = RangeWidget(self._gain_slider_range, self.set_gain_slider, "Gain", "counter", float)
        self.top_layout.addWidget(self._gain_slider_win)
        self.blocks_socket_pdu_0_1 = blocks.socket_pdu("UDP_CLIENT", collector, collectorport, 1500, False)
        self.blocks_socket_pdu_0_0 = blocks.socket_pdu("UDP_SERVER", "127.0.0.1", serverport, 10000, False)
        self.blocks_rotator_cc_0_0_3 = blocks.rotator_cc(-2*100e3*pi/samp_rate)
        self.blocks_rotator_cc_0_0_2 = blocks.rotator_cc(-2*100e3*pi/samp_rate)
        self.blocks_rotator_cc_0_0_1 = blocks.rotator_cc(-2*100e3*pi/samp_rate)
        self.blocks_rotator_cc_0_0_0 = blocks.rotator_cc(-2*100e3*pi/samp_rate)
        self.blocks_rotator_cc_0_0 = blocks.rotator_cc(-2*100e3*pi/samp_rate)
        self.blocks_null_sink_2 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_1 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.gsm_bcch_ccch_demapper_0, 'bursts'), (self.gsm_control_channels_decoder_0, 'bursts'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.blocks_socket_pdu_0_1, 'pdus'))    
        self.msg_connect((self.gsm_control_channels_decoder_0, 'msgs'), (self.gsm_message_printer_1, 'msgs'))    
        self.msg_connect((self.gsm_receiver_0, 'C0'), (self.gsm_bcch_ccch_demapper_0, 'bursts'))    
        self.connect((self.blocks_rotator_cc_0_0, 0), (self.multichannel_file_source_0, 0))    
        self.connect((self.blocks_rotator_cc_0_0_0, 0), (self.multichannel_file_source_0, 2))    
        self.connect((self.blocks_rotator_cc_0_0_1, 0), (self.multichannel_file_source_0, 1))    
        self.connect((self.blocks_rotator_cc_0_0_2, 0), (self.multichannel_file_source_0, 3))    
        self.connect((self.blocks_rotator_cc_0_0_3, 0), (self.multichannel_file_source_0, 4))    
        self.connect((self.gsm_file_source_C0_0, 0), (self.blocks_rotator_cc_0_0, 0))    
        self.connect((self.gsm_file_source_C0_0_0, 0), (self.blocks_rotator_cc_0_0_1, 0))    
        self.connect((self.gsm_file_source_C0_0_0_0, 0), (self.blocks_rotator_cc_0_0_0, 0))    
        self.connect((self.gsm_file_source_C0_0_0_0_0, 0), (self.blocks_rotator_cc_0_0_2, 0))    
        self.connect((self.gsm_file_source_C0_0_0_0_0_0, 0), (self.blocks_rotator_cc_0_0_3, 0))    
        self.connect((self.gsm_input_1, 0), (self.gsm_receiver_0, 0))    
        self.connect((self.multichannel_file_source_0, 1), (self.blocks_null_sink_0, 0))    
        self.connect((self.multichannel_file_source_0, 2), (self.blocks_null_sink_1, 0))    
        self.connect((self.multichannel_file_source_0, 0), (self.blocks_null_sink_2, 0))    
        self.connect((self.multichannel_file_source_0, 1), (self.gsm_input_1, 0))    

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

    def get_gain(self):
        return self.gain

    def set_gain(self, gain):
        self.gain = gain
        self.set_gain_slider(self.gain)

    def get_osr(self):
        return self.osr

    def set_osr(self, osr):
        self.osr = osr
        self.set_samp_rate_out(1.083333333e6*self.osr/4)
        self.gsm_input_1.set_osr(self.osr)

    def get_ppm(self):
        return self.ppm

    def set_ppm(self, ppm):
        self.ppm = ppm
        self.set_ppm_slider(self.ppm)
        self.gsm_input_1.set_ppm(self.ppm)

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_rotator_cc_0_0.set_phase_inc(-2*100e3*pi/self.samp_rate)
        self.blocks_rotator_cc_0_0_0.set_phase_inc(-2*100e3*pi/self.samp_rate)
        self.blocks_rotator_cc_0_0_1.set_phase_inc(-2*100e3*pi/self.samp_rate)
        self.blocks_rotator_cc_0_0_2.set_phase_inc(-2*100e3*pi/self.samp_rate)
        self.blocks_rotator_cc_0_0_3.set_phase_inc(-2*100e3*pi/self.samp_rate)

    def get_serverport(self):
        return self.serverport

    def set_serverport(self, serverport):
        self.serverport = serverport

    def get_shiftoff(self):
        return self.shiftoff

    def set_shiftoff(self, shiftoff):
        self.shiftoff = shiftoff

    def get_c0(self):
        return self.c0

    def set_c0(self, c0):
        self.c0 = c0
        self.set_fc(arfcn.arfcn2downlink(self.c0))

    def get_fc(self):
        return self.fc

    def set_fc(self, fc):
        self.fc = fc
        self.set_fc_slider(self.fc)
        self.gsm_input_1.set_fc(self.fc)

    def get_start_900(self):
        return self.start_900

    def set_start_900(self, start_900):
        self.start_900 = start_900

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
        self.qtgui_freq_sink_x_0.set_frequency_range(self.fc_slider, 200e3)

    def get_bandwidth_900(self):
        return self.bandwidth_900

    def set_bandwidth_900(self, bandwidth_900):
        self.bandwidth_900 = bandwidth_900


def argument_parser():
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    parser.add_option(
        "", "--args", dest="args", type="string", default="",
        help="Set Device Arguments [default=%default]")
    parser.add_option(
        "", "--collector", dest="collector", type="string", default="localhost",
        help="Set IP or DNS name of collector point [default=%default]")
    parser.add_option(
        "", "--collectorport", dest="collectorport", type="string", default="4729",
        help="Set UDP port number of collector [default=%default]")
    parser.add_option(
        "-g", "--gain", dest="gain", type="eng_float", default=eng_notation.num_to_str(30),
        help="Set gain [default=%default]")
    parser.add_option(
        "", "--osr", dest="osr", type="intx", default=2,
        help="Set OverSampling Ratio [default=%default]")
    parser.add_option(
        "-p", "--ppm", dest="ppm", type="eng_float", default=eng_notation.num_to_str(0),
        help="Set ppm [default=%default]")
    parser.add_option(
        "-s", "--samp-rate", dest="samp_rate", type="eng_float", default=eng_notation.num_to_str(2000000.052982),
        help="Set samp_rate [default=%default]")
    parser.add_option(
        "", "--serverport", dest="serverport", type="string", default="4729",
        help="Set UDP server listening port [default=%default]")
    parser.add_option(
        "-o", "--shiftoff", dest="shiftoff", type="eng_float", default=eng_notation.num_to_str(400e3),
        help="Set Frequency Shiftoff [default=%default]")
    return parser


def main(top_block_cls=grgsm_livemon, options=None):
    if options is None:
        options, _ = argument_parser().parse_args()

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls(args=options.args, collector=options.collector, collectorport=options.collectorport, gain=options.gain, osr=options.osr, ppm=options.ppm, samp_rate=options.samp_rate, serverport=options.serverport, shiftoff=options.shiftoff)
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()


if __name__ == '__main__':
    main()
