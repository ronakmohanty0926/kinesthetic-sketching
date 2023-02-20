function VisualizeData()
A=load('UserDATA.txt');

figure;
hold on
for i=1:length(A)
	
        line([A(i,2),A(i+1,2)],[A(i,3),A(i+1,3)],[A(i,4),A(i+1,4)],'Color','blue','LineStyle','-');
   
	
end
hold off;