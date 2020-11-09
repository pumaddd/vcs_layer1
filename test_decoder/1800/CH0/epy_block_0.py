"""
Embedded Python Blocks:

Each this file is saved, GRC will instantiate the first class it finds to get
ports and parameters of your block. The arguments to __init__  will be the
parameters. All of them are required to have default values!
"""
from gnuradio import gr
from gnuradio import blocks
import pmt


class blk(gr.basic_block):  # other base classes are basic_block, decim_block, interp_block
    """This block converts strings from the QT GUI Message Edit Box to uint8 vectors"""

    def __init__(self):  # only default arguments here
        """arguments to this function show up as parameters in GRC"""

        gr.basic_block.__init__(
            self,
            name='Embedded Python Block',  # will show up in GRC
            in_sig=None,
            out_sig=None)
        # self.call_back = self.handle_msg
        self.message_port_register_in(pmt.intern("inmsg_in"))
        self.set_msg_handler(pmt.intern("inmsg_in"), self.handle_msg)

    def handle_msg(self, msg):
        print "hello world: ", msg


    def work(self, input_items, output_items):
        pass

    def set_callback(self, callback):
        if callback is not None:
            self.set_msg_handler(pmt.intern("inmsg_in"), callback)
        else:
            self.set_msg_handler(pmt.intern("inmsg_in"), self.handle_msg)

