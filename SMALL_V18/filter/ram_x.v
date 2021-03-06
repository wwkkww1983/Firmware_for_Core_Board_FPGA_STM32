module delta_limit(clk,rst_n,enable,xin,xout,address);
input clk,rst_n,enable;
input [31:0]xin;
input[6:0]address;
output [31:0]xout;
reg [31:0]xout;
reg [2111:0]x;
reg lastenable;
always @(posedge clk or negedge rst_n)
if(!rst_n)
    begin
    xout<=0;
    x<=0;
    end
else
	begin
	lastenable <= enable;
	    if(enable==1'b1 && lastenable == 1'b0)
	    begin
	        x[2111:32] <= x [2079:0];
	        x[31:0] <= xin;
	        xout <= xin;
	    end
	    else
	    begin
	        xout <= x[((address+1)*32 - 1):(address*32-1)];
	    end
	end
endmodule
	    