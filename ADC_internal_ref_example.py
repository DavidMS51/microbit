# Code to demo using the microbit internal ADC reference 
# based on material and help from Carlos Pereira Atencio  https://gist.github.com/microbit-carlos/
# David Saul 2017. Released to the public domain.

from microbit import *

NRF_ADC_BASE = 0x40007000
NRF_ADC_CONFIG = NRF_ADC_BASE + 0x504

# functions used to modify ADC setup
@micropython.asm_thumb
def reg_read(r0):
    ldr(r0, [r0, 0])

@micropython.asm_thumb
def reg_bit_clear(r0, r1):
    ldr(r2, [r0, 0])
    bic(r2, r1)
    str(r2, [r0, 0])
    
    
# change ADC config for pin0

# mbed configured: 10bit, input 1/3, ref 1/3 vdd, enable ADC in AIN4-P0.03-pin0
pin0.read_analog()   
#Set ref voltage to internal ref 1.2V (band gap), set bits 5-6 to 0x00
reg_bit_clear(NRF_ADC_CONFIG, 0x60)
print("ADC config should be 0x100A: {:#010x}".format(reg_read(NRF_ADC_CONFIG)))

#variable definition
vt = 0          # raw / scaled analoge value
vf = "0000"     # mesured voltage as a formatted string 
cal = 352      # calibration variable
                # cal is calculated as (Vin / raw count)*10000
               

while True:
    vt = pin0.read_analog()
    print ('raw count = ',vt)
    
    # the following lines of code scale the measured value and derive a
    # string (vf) that equates to it - there are simpler ways to do this
    # but this method avoids the use of floating varaibles which are very
    # inefficent on cards like the microbit
    
    vt = int((vt * cal)/1000) 
    vs = str(vt)
    if vt > 1000:
        vf = vs[0:2]+'.'+vs[2:3] 
    elif vt > 100 :
        vf = vs[0:1]+'.'+vs[1:3]
    elif vt > 10 :
        vf = '0.'+vs
    elif vt > 1 :
        vf = '0.0'+vs
    else :
        vf = '0.000'   
    
    vf = vf+"V"
    print ('Scaled', vf)
    display.scroll(vf)
    
    sleep(2000)