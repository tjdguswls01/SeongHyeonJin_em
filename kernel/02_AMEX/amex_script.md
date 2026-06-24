# AMEX Kernel 4의 이해 발표 대본

## 슬라이드 1. 제목
이번 발표는 American Express 비접촉 결제에서 Kernel 4가 EMV, Mag-Stripe, Mobile 모드를 구분하고 첫 Card Action Analysis까지 처리하는 구조를 설명한다.

## 슬라이드 2. AMEX Kernel 4는 무엇인가
Kernel 4는 AMEX 비접촉 결제를 위한 L2 처리 엔진이며 카드 응답을 해석하고 거래 방향을 결정한다.

## 슬라이드 3. 시스템 안에서 Kernel 4의 위치
Entry Point 이후부터 카드·단말 데이터를 바탕으로 결과를 만들기 전까지의 규칙을 담당한다.

## 슬라이드 4. Kernel 4 전체 흐름
GPO에서 모드를 결정하고, 데이터 읽기와 ODA, Restrictions, CVM, Risk, 첫 Terminal Action Analysis, 첫 GENERATE AC 순서로 진행한다. 비접촉 거래는 여기서 카드와의 처리가 끝나며 두 번째 GENERATE AC는 사용하지 않는다.

## 슬라이드 5. Kernel 4가 보는 거래 조합
EMV Mode는 ODA와 위험관리, Mag-Stripe Mode는 트랙 데이터와 동적 값, Mobile 거래는 Tag 9F71을 중심으로 처리한다. 완료 방식은 Offline, Partial Online, Delayed Authorisation으로 다시 나뉜다.

## 슬라이드 6. PDOL 조립과 GPO 요청
단말 데이터는 PDOL 형식에 맞춰 조립되고 GPO 명령을 통해 카드 Capability 확인으로 이어진다.

## 슬라이드 7. AIP와 AFL이 의미하는 것
AIP는 카드 기능을, AFL은 어떤 레코드를 읽을지를 알려 주는 기준 데이터이다.

## 슬라이드 8. Terminal Type과 Reader Capability
단말 환경과 리더 기능 선언이 카드의 처리 방향 판단에 직접 영향을 준다.

## 슬라이드 9. AFL 기반 Read Application Data
AFL이 지시한 범위대로 카드 데이터를 읽어 후속 검증과 판단에 필요한 자료를 확보한다.

## 슬라이드 10. 읽은 데이터가 중요한 이유
읽은 데이터는 단순 저장 대상이 아니라 ODA, CVM, Risk, Action Analysis의 입력이 된다.

## 슬라이드 11. ODA의 목적
ODA는 위조 카드와 변조 데이터를 온라인 없이도 걸러 내기 위한 공개키 기반 검증이다.

## 슬라이드 12. ODA에서 보는 것
CA Key, Issuer·ICC 인증서, CDA 결과 같은 요소를 검증해 카드 진위를 판단한다.

## 슬라이드 13. Processing Restrictions의 의미
이 단계는 승인 결론을 내리기보다는 거래 제약 조건과 위험 신호를 TVR에 누적하는 단계이다.

## 슬라이드 14. Dynamic Reader Limits
Transaction Limit, CVM Required Limit, Floor Limit은 각각 다른 결정을 제어한다.

## 슬라이드 15. CVM 판단 구조
카드 Capability, Reader Capability, 금액, 한도 조건을 함께 보고 최종 CVM을 정한다.

## 슬라이드 16. Mobile CVM은 왜 별도로 보아야 하는가
9F71 결과만 보는 것이 아니라 Reader Limit과 Context까지 함께 맞아야 Mobile CVM이 성립한다.

## 슬라이드 17. Terminal Risk Management
Floor Limit, Random Selection, Velocity, Exception 조건으로 온라인 필요성과 위험 수준을 판단한다.

## 슬라이드 18. Risk 결과는 어떻게 쓰이는가
각 결과는 TVR 비트에 누적되고 이후 IAC·TAC 비교의 직접 입력이 된다.

## 슬라이드 19. 1st Terminal Action Analysis
Denial 우선, 그다음 Online 여부, 마지막으로 Offline 승인 가능 여부 순서로 AC 요청 방향을 정한다.

## 슬라이드 20. 1st GENERATE AC와 응답 해석
단말이 요청한 AC와 카드가 실제로 반환한 AC를 다시 해석해 의미를 확인해야 한다.

## 슬라이드 21. Online Processing은 카드 제거 후 이어진다
첫 Card Action Analysis가 끝나면 카드를 제거하도록 안내한다. ARQC, ATC, IAD, TVR과 CVM 결과는 Issuer로 전송되며 Partial Online 거래의 최종 승인과 거절은 Issuer 응답으로 정한다.

## 슬라이드 22. Partial Online과 Delayed Authorisation
Partial Online은 카드 처리를 끝낸 뒤 즉시 Issuer 응답으로 완료한다. Delayed Authorisation은 즉시 연결이 어려운 환경에서 첫 GENERATE AC 결과를 보관하고 나중에 승인을 요청한다. 둘 다 두 번째 GENERATE AC를 사용하지 않는다.

## 슬라이드 23. Zero Amount와 Status Check
특수 거래는 일반 구매와 다른 정책을 적용해야 하며 Modified Terminal Type 같은 보정 개념이 중요하다.

## 슬라이드 24. Outcome과 최종 단말 동작
Kernel은 Outcome과 Data Record를 반환하고 상위 앱은 그 결과를 실제 서비스 동작으로 바꾼다.

## 슬라이드 25. TNP-10 소스에서의 구현 위치와 결론
설정, 선택, 거래 중심 처리 함수가 AMEX Kernel 4의 Capability, 검증, 위험, 승인 판단 흐름을 구현한다.
