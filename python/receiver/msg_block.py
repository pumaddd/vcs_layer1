from gnuradio import gr
from gnuradio import blocks
import pmt
class msg_block(gr.basic_block):  # other base classes are basic_block, decim_block, interp_block
    """This block converts strings from the QT GUI Message Edit Box to uint8 vectors"""

    def __init__(self):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""
	gr.basic_block.__init__(            
	    self,
            name='Message Handle',   # will show up in GRC
            in_sig=None,
            out_sig=None)
        self.message_port_register_in(pmt.intern("msg_in"))
        self.set_msg_handler(pmt.intern("msg_in"), self.handle_msg)
    
    def handle_msg(self, msg):
	print "hello world: ", msg
