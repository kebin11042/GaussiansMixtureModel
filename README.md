# GaussiansMixtureModel
Mixture Of Gaussian을 이용한 전경 배경 구분

개요

 주위 환경 변화에 적응 가능한 가우시안 혼합 모델(Gaussian Mixture Models)을 이용하여 전경(움직이는 객체)와 배경의 분리


목표설정

 - 가우시안 혼합 모델(GMM)
   연산의 초기값과 수식을 앞서 언급한 논문을 바탕으로 구현.

 - Median Filter
   영상의 화소와 처리 시간에 맞게 3*3 마스크를 이용하여 구현.

 - Image Labeling
   이용방법이 간단하고 재귀 함수에 의한 연결영역 검색 방법보다 처리 시간이 빠른
   4-neighborhood two-pass 방식의 Labeling을 구현.
   영상마다 다양한 객체가 존재하기 때문에 Labeling Size를 사용자가 입력할 수 있도록
   구현.
   Labeling이 적용된 각 객체에 사용자가 쉽게 볼 수 있도록 두께 2 px 빨간색 테두리를
   적용


참고문헌 

 P. W. Power and J. A. Schoonees, "Understanding Background Mixture Models for Foreground Segmentation", Proc. of Image and Vision Computing, Auckland, New Zealand, pp.267-271, Nov. 2002
