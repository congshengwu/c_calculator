#include "stdio.h"
#include "string.h"
#define LEN 99//计算式最大长度,可以自定义

/*
 *	1.C语言计算式(字符串形式)求解函数.
 *	2.支持符号:0~9 + - * / . ( ) 注:小括号可以是n级
 *  3.负数在计算式中需要这样表示,如:-6*5 >>>>>> (0-6)*5
 *  4.需要保持式子的正确性和完整性,不然程序有可能报错
 *  5.原理是用了三个栈,指针不断分析字符串里每个字符,
 *	  不断存进三个栈中,一个存数字的个位十位等每一位，一个存运算符，一个存数字. 
 */
double calculate(char *c){
	//c是存计算式字符串的指针
	double result=0;//最终返回变量
	int c_len=strlen(c);
	int i,k;//i当过数组下标
	double temp,j;
	double opnd[LEN];//操作数栈,能存10个操作数,包括小数点
	int opnd_index=0;
	char optr[LEN]="";//操作符栈
	optr[0]=0;
	int optr_index=0;
	/*当 ) 进入栈的时候，会将 (……)里的计算式都处理了,
	左栈存一个两个数结合的新数,右栈指针指向(,
	紧接着再右栈再输入 +-×÷* 就不许先不需要计算了,直接替换 ) */
	int CalByButton_r=0;//等于0是 ) 处理式子之前
	char num_stack[LEN]="";//临时数栈
	int num_stack_index=0;//临时数下标
	double num_temp=0;//临时数
	
	//注意：while的最后一次运行是 数字的，所以数字下面的判断都不会执行
	while(c_len) {//+-×÷.肯不会在字符串开头 (可能在开头，需要处理
		c_len--;
		//*c是数字情况，包括小数点
		if((*c<='9' && *c>='0') || (*c=='.')) {
			num_stack[num_stack_index]=*c;
			num_stack_index++;
			c++;
			continue;
		}
		//*c是 + 情况
		if(*c=='+') {
			num_stack_index--;
			if(num_stack_index==-1){// ) 已经提取一遍左面了，) 后的运算符 就不用再提左面了
				num_stack_index=0;
				goto c1;
			}
			//将+前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
			for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
				if(num_stack[i] != '.')
					num_temp+=((num_stack[i]-'0')*j);
				
				if(num_stack[i]=='.'){
					for(k=num_stack_index-i;k>0;k--) {
						num_temp/=10;
					}
					j=0.1;//下一次j还是1
				}
			}
			opnd[opnd_index]=num_temp;
			opnd_index++;
			num_temp=0;//临时数初始化
			num_stack_index=0;//临时栈下标初始化
			c1:
			//存 + 的一些操作
			if(optr[optr_index]==0) {//如果optr为空,直接进入，之后直到到最终计算之前都不会为空了
				optr[optr_index]='+';//先不++，因为下面是判断上一个入栈负号
			}//如果optr不为空，+ 是 直接抽出里面的进行运算，再存
			else if(optr[optr_index]=='+') {//左提俩求和=新数，将新数存进opnd,+号不错废掉
				opnd_index-=2;//减去多加的 和 二合一之后位置为下面那个
				temp=opnd[opnd_index]+opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;//
				
				optr[optr_index]='+';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='-') {
				opnd_index-=2;
				temp=opnd[opnd_index]-opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
				optr[optr_index]='+';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='*') {//+遇到
				opnd_index-=2;
				temp=opnd[opnd_index]*opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				while(1){//处理 6+8-4*2+6-10 的 bug
					if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){
							case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
						}
						opnd_index++;
					}
					break;
				}
				optr[optr_index]='+';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='/') {
				opnd_index-=2;
				temp=opnd[opnd_index]/opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
				while(1){//处理 6+8-4/2+6-10 的 bug
					if (optr_index - 1 < 0)//防止数组越界
                        break;
					if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){
							case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
						}
						opnd_index++;
					}
					break;
				}
				optr[optr_index]='+';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='('){//如果是 ( 直接存进去
				if(CalByButton_r==1){//这个情况是遇到 由 ) 计算后 的 ( ; 不是输入 ) 前的 (
					if((optr_index-1)>=0){//处理 6*(1-2*2)+3会=0 的问题 存之前应该再判断一下 原 ( 下的符号*/											
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){ //这是运算符栈顶符号，此时 ) 没存进去
							case '+':opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-':opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
							case '*':opnd[opnd_index]=opnd[opnd_index]*opnd[opnd_index+1];break;
							case '/':opnd[opnd_index]=opnd[opnd_index]/opnd[opnd_index+1];break;
							//下面这个case是处理 ( 下还是 ( 的情况 
							case '(':optr_index++;opnd_index++;break;//只需将optr_index和opnd_index归位即可							
						}
						opnd_index++;
						optr[optr_index]='+';
						CalByButton_r=0;//归位
						while(1){
							if (optr_index - 1 < 0)//防止数组越界
								break;
							if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
								optr_index--;
								opnd_index-=2;
								switch(optr[optr_index]){
									case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
									case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
								}
							opnd_index++;
							}
							optr[optr_index]='+';
							break;
						}
						c++;
						continue;
					}
					optr[optr_index]='+';
					CalByButton_r=0;//归位
					c++;
					continue;
				}
				optr_index++;
				optr[optr_index]='+';
			}
			c++;
			continue;
		}
		//*c是 - 情况，和 + 一样  负号处理在 符号(中
		if(*c=='-') {
			num_stack_index--;
			if(num_stack_index==-1){// ) 已经提取一遍左面了，) 后的运算符 就不用再提左面了
				num_stack_index=0;
				goto c2;
			}
			//将-前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
			for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
				if(num_stack[i] != '.')
					num_temp+=((num_stack[i]-'0')*j);
				
				if(num_stack[i]=='.'){
					for(k=num_stack_index-i;k>0;k--) {
						num_temp/=10;
					}
					j=0.1;//下一次j还是1
				}
			}
			opnd[opnd_index]=num_temp;
			opnd_index++;
			num_temp=0;//临时数初始化
			num_stack_index=0;//临时栈下标初始化
			c2:
			//存 - 的一些操作
			if(optr[optr_index]==0) {//如果optr为空,直接进入
				optr[optr_index]='-';//先不++，因为下面是判断上一个入栈负号
			}//如果optr不为空，+ 是 直接抽出里面的进行运算，再存
			else if(optr[optr_index]=='+') {//左提俩求和=新数，将新数存进opnd,+号不错废掉
				opnd_index-=2;//减去多加的 和 二合一之后位置为下面那个
				temp=opnd[opnd_index]+opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;//
				
				optr[optr_index]='-';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='-') {
				opnd_index-=2;
				temp=opnd[opnd_index]-opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
				optr[optr_index]='-';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='*') {//+遇到
				opnd_index-=2;
				temp=opnd[opnd_index]*opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				while(1){//处理 6+8-4/2+6-10 的 bug
					if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){
							case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
						}
						opnd_index++;
					}
					break;
				}
				optr[optr_index]='-';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='/') {
				opnd_index-=2;
				temp=opnd[opnd_index]/opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				while(1){//处理 6+8-4/2+6-10 的 bug
					if (optr_index - 1 < 0)//防止数组越界
                        break;
					if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){
							case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
						}
						opnd_index++;
					}
					break;
				}
				optr[optr_index]='-';//直接替换掉原来下标为这个的符号
			}else if(optr[optr_index]=='('){//如果是 ( 直接存进去
				if(CalByButton_r==1){//这个情况是 由 ) 计算后 的 ( 遗留
					if((optr_index-1)>=0){//处理 6*(1-2*2)+3会=0 的问题 /*存之前应该再判断一下 原 ( 下的符号*/	
						optr_index--;
						opnd_index-=2;
						switch(optr[optr_index]){ //这是运算符栈顶符号，此时 ) 没存进去
							case '+':opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
							case '-':opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
							case '*':opnd[opnd_index]=opnd[opnd_index]*opnd[opnd_index+1];break;
							case '/':opnd[opnd_index]=opnd[opnd_index]/opnd[opnd_index+1];break;
							case '(':optr_index++;opnd_index++;break;
							//如果是 ( 下还是 ( 只需将optr_index和opnd_index归位即可
						}
						opnd_index++;
						optr[optr_index]='-';
						CalByButton_r=0;//归位
						c++;
						continue;
					}
					optr[optr_index]='-';
					CalByButton_r=0;//归位
					while(1){
						if (optr_index - 1 < 0)//防止数组越界
							break;
						if(optr[optr_index-1]=='+' || optr[optr_index-1]=='-'){
							optr_index--;
							opnd_index-=2;
							switch(optr[optr_index]){
								case '+': opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
								case '-': opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
							}
							opnd_index++;
						}
						optr[optr_index]='-';
						break;
					}
					c++;
					continue;
				}
				optr_index++;
				optr[optr_index]='-';
			}
			c++;
			continue;
		}
		//*c是 × 情况
		if(*c=='*') {
			num_stack_index--;
			if(num_stack_index==-1){// ) 已经提取一遍左面了，) 后的运算符 就不用再提左面了
				num_stack_index=0;
				goto c3;
			}
			//将*前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
			for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
				if(num_stack[i] != '.')
					num_temp+=((num_stack[i]-'0')*j);
				
				if(num_stack[i]=='.'){
					for(k=num_stack_index-i;k>0;k--) {
						num_temp/=10;
					}
					j=0.1;//下一次j还是1
				}
			}
			opnd[opnd_index]=num_temp;
			opnd_index++;
			num_temp=0;//临时数初始化
			num_stack_index=0;//临时栈下标初始化
			//存 * 的一些操作
			c3:
			if(optr[optr_index]==0) {//如果optr为空,直接进入
				optr[optr_index]='*';//先不++，因为下面是判断上一个入栈负号
			}//如果optr不为空
			else if(optr[optr_index]=='+') {//遇到+号不运算直接存进去
				optr_index++;
				optr[optr_index]='*';
			}else if(optr[optr_index]=='-') {
				optr_index++;
				optr[optr_index]='*';
			}else if(optr[optr_index]=='*') {//*遇到*,左提俩=新数，新数放入,*放入原来*位置,也就是不用操作
				opnd_index-=2;
				temp=opnd[opnd_index]*opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
			}else if(optr[optr_index]=='/') {
				opnd_index-=2;
				temp=opnd[opnd_index]/opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
				optr[optr_index]='*';//*替换/的位置
			}else if(optr[optr_index]=='('){//如果是 ( 直接存进去
				if(CalByButton_r==1){//这个情况是 由 ) 计算后 的 (
					if((optr_index-1)>=0){//处理 6*(1-2*2)+3会=0 的问题 /*存之前应该再判断一下 原 ( 下的符号*/	
						if(optr[optr_index-1]!='+' && optr[optr_index-1]!='-'){//处理6-(5+2)*6 *入之前 把 - 号给算了
							optr_index--;
							opnd_index-=2;
							switch(optr[optr_index]){ //这是运算符栈顶符号，此时 ) 没存进去
								case '+':opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
								case '-':opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
								case '*':opnd[opnd_index]=opnd[opnd_index]*opnd[opnd_index+1];break;
								case '/':opnd[opnd_index]=opnd[opnd_index]/opnd[opnd_index+1];break;
								case '(':optr_index++;opnd_index++;break;
							//如果是 ( 下还是 ( 只需将optr_index和opnd_index归位即可
							}
							opnd_index++;
							optr[optr_index]='*';
							CalByButton_r=0;//归位
							c++;
							continue;
						}
					}
					optr[optr_index]='*';
					CalByButton_r=0;//归位
					c++;
					continue;
				}
				optr_index++;
				optr[optr_index]='*';
			}
			c++;
			continue;
		}
		//*c是 / 情况
		if(*c=='/') {
			num_stack_index--;
			if(num_stack_index==-1){// ) 已经提取一遍左面了，) 后的运算符 就不用再提左面了
				num_stack_index=0;
				goto c4;
			}
			//将*前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
			for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
				if(num_stack[i] != '.')
					num_temp+=((num_stack[i]-'0')*j);
				
				if(num_stack[i]=='.'){
					for(k=num_stack_index-i;k>0;k--) {
						num_temp/=10;
					}
					j=0.1;//下一次j还是1
				}
			}
			opnd[opnd_index]=num_temp;
			opnd_index++;
			num_temp=0;//临时数初始化
			num_stack_index=0;//临时栈下标初始化
			c4:
			//存 / 的一些操作
			if(optr[optr_index]==0) {//如果optr为空,直接进入
				optr[optr_index]='/';//先不++，因为下面是判断上一个入栈负号
			}//如果optr不为空
			else if(optr[optr_index]=='+') {//遇到+号不运算直接存进去
				optr_index++;
				optr[optr_index]='/';
			}else if(optr[optr_index]=='-') {
				optr_index++;
				optr[optr_index]='/';
			}else if(optr[optr_index]=='*') {
				opnd_index-=2;
				temp=opnd[opnd_index]*opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
				
				optr[optr_index]='/';// /替换*的位置
			}else if(optr[optr_index]=='/') {// /遇到/,左提俩=新数，新数放入,/放入原来/位置,也就是不用操作
				opnd_index-=2;
				temp=opnd[opnd_index]/opnd[opnd_index+1];
				opnd[opnd_index]=temp;
				opnd_index++;
			}else if(optr[optr_index]=='('){//如果是 ( 直接存进去
				if(CalByButton_r==1){//这个情况是 由 ) 计算后 的 (					
					if((optr_index-1)>=0){//处理 6*(1-2*2)+3会=0 的问题 /*存之前应该再判断一下 原 ( 下的符号*/	
						if(optr[optr_index-1]!='+' && optr[optr_index-1]!='-'){//处理6-(5+2)/6 /入之前 把 - 号给算了
							optr_index--;
							opnd_index-=2;
							switch(optr[optr_index]){ //这是运算符栈顶符号，此时 ) 没存进去
								case '+':opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
								case '-':opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
								case '*':opnd[opnd_index]=opnd[opnd_index]*opnd[opnd_index+1];break;
								case '/':opnd[opnd_index]=opnd[opnd_index]/opnd[opnd_index+1];break;
								case '(':optr_index++;opnd_index++;break;
							//如果是 ( 下还是 ( 只需将optr_index和opnd_index归位即可
							}
							opnd_index++;
							optr[optr_index]='/';
							CalByButton_r=0;//归位
							c++;
							continue;
						}
					}
					optr[optr_index]='/';
					CalByButton_r=0;//归位
					c++;
					continue;
				}
				optr_index++;
				optr[optr_index]='/';
			}
			c++;
			continue;
		}
		//是 ( 情况
		if(*c=='('){//直接入操作符栈
			if(optr_index==0){
				if(optr[0]!='+' && optr[0]!='-' && optr[0]!='*' && optr[0]!='/' && optr[0]!='('){
					optr[optr_index]='(';
					c++;
					continue;
				}
			}
			optr_index++;
			optr[optr_index]='(';
			c++;
			continue;
		}
		//是 ) 情况
		if(*c==')'){
			num_stack_index--;
			//将*前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
			for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
				if(num_stack[i] != '.')
					num_temp+=((num_stack[i]-'0')*j);
				
				if(num_stack[i]=='.'){
					for(k=num_stack_index-i;k>0;k--) {
						num_temp/=10;
					}
					j=0.1;//下一次j还是1
				}
			}
			opnd[opnd_index]=num_temp;
			opnd_index++;
			num_temp=0;//临时数初始化
			num_stack_index=0;//临时栈下标初始化
			
			while(1){//用while是处理 这种情况 (1-5*2) 也就是括号内的多项式
				opnd_index-=2;
				switch(optr[optr_index]){ //这是运算符栈顶符号，此时 ) 没存进去
					case '+':opnd[opnd_index]=opnd[opnd_index]+opnd[opnd_index+1];break;
					case '-':opnd[opnd_index]=opnd[opnd_index]-opnd[opnd_index+1];break;
					case '*':opnd[opnd_index]=opnd[opnd_index]*opnd[opnd_index+1];break;
					case '/':opnd[opnd_index]=opnd[opnd_index]/opnd[opnd_index+1];break;
				}
				opnd_index++;
				optr_index--;//运算符栈下降一位
				if(optr[optr_index]=='('){
					CalByButton_r=1; //上面有说明
					break;
				}
				
			}
		
		}
		
		c++;//指针在最后时刻移动
	}
	
	///////////////////////////////////////////
		num_stack_index--;
		//将+前一个数抽出存入opnd，num_temp是从字符串中成功抽取的一个数,并且已经变成double
		for(i=num_stack_index,j=1; i>=0; i--,j*=10) {
			if(num_stack[i] != '.')
				num_temp+=((num_stack[i]-'0')*j);
			
			if(num_stack[i]=='.'){
				for(k=num_stack_index-i;k>0;k--) {
					num_temp/=10;
				}
				j=0.1;//下一次j还是1
			}
		}
		opnd[opnd_index]=num_temp;
		opnd_index++;
		num_temp=0;//临时数初始化
		num_stack_index=0;//临时栈下标初始化
	
	i=optr_index;
	do{
		switch(optr[i]){
			case '+':opnd[i]=opnd[i]+opnd[i+1];break;
			case '-':opnd[i]=opnd[i]-opnd[i+1];break;
			case '*':opnd[i]=opnd[i]*opnd[i+1];break;
			case '/':opnd[i]=opnd[i]/opnd[i+1];break;
		}
		i--;
	}while(i!=-1);
	result=opnd[0];
	
	return result;
}

int main() {
	
	while(1){
		char c[100];
		printf("算式: ");
		scanf("%s",c);
		if(c[0] == 'q')//按q退出程序
			return 0;
		double result = calculate(c);//此处调用
		printf("结果: %0.10f\n",result);//输出保留小数点后十位
	}
	
	return 0;
}