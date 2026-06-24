# Mastercard Kernel 2 발표 대본
> 슬라이드 25장 기준

## 슬라이드 1
오늘 발표 주제는 Mastercard Kernel 2의 이해다. 이 발표에서는 개별 PDF 장별 나열보다, 실제 거래가 어떤 구조와 어떤 데이터로 흘러가는지 중심으로 설명한다. 핵심은 Signal 구조, TLV Database, 두 가지 거래 모드, 그리고 최종 Outcome 생성 방식이다.

## 슬라이드 2
Kernel 2는 Mastercard 비접촉 결제를 위한 Level 2 처리 계층이다. 카드와 단말기 사이에 오가는 명령을 단순 전달하는 것이 아니라, 어떤 순서로 읽고 어떤 조건에서 승인 또는 온라인 요청을 할지 판단한다. 즉, 브랜드 규칙을 실제 거래 로직으로 바꾸는 엔진이라고 보면 된다.

## 슬라이드 3
POS 시스템 전체에서 보면 Terminal은 금액 입력과 화면 처리, 온라인 승인 연동을 맡는다. Entry Point는 카드가 어떤 브랜드인지 보고 적절한 커널을 고른다. 그 다음 Kernel 2가 Mastercard 규칙에 따라 거래를 해석하고, 하위 계층은 APDU와 NFC 통신을 수행한다.

## 슬라이드 4
Reader 구조는 M, S, P, D, K의 다섯 프로세스로 설명한다. M은 흐름을 조정하고, S는 카드와 커널을 선택하고, P는 카드와의 APDU 통신을 맡고, D는 UI와 메시지를 관리한다. K가 바로 Kernel이며, 거래 판단의 핵심이 이 안에서 수행된다.

## 슬라이드 5
이들 프로세스는 함수 호출보다 Signal 기반으로 결합한다. 시작은 ACT 신호이고, 카드 명령과 응답은 CA와 RA를 통해 오가며, 최종 결과는 OUT 신호로 올라간다. 즉, Kernel 2는 독립적인 판단 모듈이지만 주변 프로세스와 느슨하게 연결된 구조를 가진다.

## 슬라이드 6
DEK와 DET는 거래 도중 단말기와 Kernel이 추가 정보를 주고받는 통로다. Kernel이 더 필요한 정보를 요청하면 DEK를 사용하고, Terminal이 응답하거나 온라인 결과를 전달할 때 DET를 사용한다. 이 구조 덕분에 거래는 카드 응답만으로 닫히지 않고 단말기 정책과도 계속 연결된다.

## 슬라이드 7
Reader Database는 영구 데이터와 임시 데이터로 나뉜다. 영구 데이터는 여러 거래에서 공통으로 쓰는 설정과 키, 메시지 같은 값이다. 임시 데이터는 현재 거래 동안만 유지되며, 카드 응답과 단말기 입력을 반영해 계속 바뀐다.

## 슬라이드 8
Kernel 2의 중심 저장 구조는 TLV Database다. 태그, 길이, 값으로 구성되기 때문에 데이터 의미가 명확하고 확장성이 좋다. 거래 시작 시 기본 인스턴스를 만들고, 이후 GPO 응답, 레코드 읽기, Generate AC 응답 같은 이벤트가 발생할 때마다 이 데이터베이스가 갱신된다.

## 슬라이드 9
거래 방향을 결정하는 기본 설정값도 중요하다. CVM Required Limit, Contactless Limit, Floor Limit는 인증 필요, 비접촉 허용, 오프라인 승인 범위를 각각 정한다. Kernel Configuration은 EMV·Mag-Stripe Mode, On-device CVM, RRP 지원을 제어한다. SDS와 IDS는 Data Exchange와 관련 데이터 객체가 추가로 준비되어야 한다.

## 슬라이드 10
Kernel 2는 크게 Mag-Stripe Mode와 EMV Mode 두 가지를 지원한다. 같은 Mastercard 비접촉 거래라도 카드 응답과 설정에 따라 경로가 달라진다. 이 구분을 이해하면 이후 데이터 흐름과 보안 수준 차이도 자연스럽게 따라온다.

## 슬라이드 11
Mag-Stripe Mode는 빠르고 단순한 구조다. GPO로 모드를 확인한 뒤 필요한 레코드를 읽고, Compute Cryptographic Checksum 명령으로 CVC3를 받아 온라인 승인용 데이터를 만든다. 구조는 단순하지만 결과는 사실상 온라인 경로로 이어진다.

## 슬라이드 12
반면 EMV Mode는 더 많은 검증을 수행한다. READ RECORD로 데이터를 충분히 모은 뒤 ODA를 수행하고, 단말기 위험관리까지 거쳐 Generate AC로 최종 방향을 결정한다. 따라서 오프라인 승인, 온라인 요청, 거절을 더 세밀하게 나눌 수 있다.

## 슬라이드 13
ODA는 카드가 진짜인지 확인하는 단계다. SDA는 정적 검증이라 구조는 단순하지만 복제 저항성은 낮다. DDA는 거래마다 다른 동적 서명을 사용하고, CDA는 동적 인증과 암호응답을 결합해 보안성과 효율을 동시에 높인다.

## 슬라이드 14
CVM과 위험관리는 Generate AC 이전의 핵심 판단 재료다. 금액과 카드 요구조건, 단말기 기능을 비교해 No CVM, Online PIN, CDCVM 같은 방법을 정한다. 동시에 TVR과 TAC, IAC 같은 비트 기반 정책을 해석해 거래를 오프라인 승인으로 갈지, 온라인 요청으로 돌릴지, 거절할지를 정한다.

## 슬라이드 15
GPO 응답으로 받는 AIP는 ODA, CVM, EMV Mode, On-device CVM, RRP 같은 카드 능력을 표시한다. Mag-Stripe Mode는 AIP의 별도 지원 비트를 읽는 방식이 아니라 EMV Mode 비트와 Kernel Configuration의 조합으로 선택한다. AFL은 레코드 순서를 정하고 IDS 데이터는 GPO Format 2의 DS 객체로 별도 전달된다.

## 슬라이드 16
READ RECORD는 실제 판단에 필요한 재료를 모으는 단계다. Track 데이터, 인증서, CDOL 관련 값, CVM 관련 값 등 이후 단계에 필요한 거의 모든 정보가 이 구간에서 확보된다. 따라서 AFL 해석 오류나 레코드 누락은 뒤 단계 전체를 흔드는 원인이 된다.

## 슬라이드 17
Generate AC는 단말기 판단과 카드 판단이 만나는 최종 단계다. 카드가 TC를 주면 오프라인 승인 방향이고, ARQC를 주면 온라인 승인 요청 방향이며, AAC를 주면 거절 방향이다. 이 응답은 단순 코드가 아니라 거래 상태를 암호적으로 표현한 결과라고 보면 된다.

## 슬라이드 18
SDS의 정확한 이름은 Standalone Data Storage이다. Data Exchange로 읽고 쓸 Tag를 받은 뒤 GET DATA와 PUT DATA를 실행한다. EMV Mode의 선택 기능이며 Mag-Stripe Mode에는 적용되지 않고, 데이터를 모두 읽은 다음 쓰기를 수행한다.

## 슬라이드 19
IDS는 SDS보다 더 진화한 방식이다. 저장 기능을 GPO와 Generate AC 흐름 안에 통합해 별도 명령을 줄이고 거래 흐름과 더 밀접하게 결합한다. 대신 DS Summary, MAC, 해시 같은 보안 장치가 함께 따라와야 한다.

## 슬라이드 20
Torn Transaction Recovery는 카드가 중간에 떨어진 상황을 복원하는 기능이다. 거래 도중 필요한 정보를 로그에 남겨 두었다가 카드가 다시 접촉하면 RECOVER AC로 이전 상태를 이어간다. 특히 오프라인 잔액이 관여하는 거래에서 일관성을 지키는 데 중요하다.

## 슬라이드 21
추가 기능으로는 모바일 거래의 CDCVM, 오프라인 잔액 조회, 그리고 Relay Resistance Protocol이 있다. CDCVM은 기기 내부 인증 결과를 거래에 반영하는 구조이고, 잔액조회는 지원 카드에 한해 별도 데이터 조회를 수행한다. RRP는 왕복 시간을 측정해 릴레이 공격 가능성을 탐지한다.

## 슬라이드 22
Kernel 2가 자주 쓰는 명령을 보면 흐름이 더 명확해진다. GPO와 READ RECORD는 거래 준비와 데이터 수집, GENERATE AC는 최종 판단, RECOVER AC는 복구, CCC는 Mag-Stripe 전용 암호값 생성에 쓰인다. SDS와 RRP 관련 명령까지 포함하면 Kernel 2의 기능 범위가 정리된다.

## 슬라이드 23
표준은 단순 기능 목록보다 상태 기계 형태로 Kernel 동작을 정의한다. 초기 상태에서 ACT를 받고, 중간 상태에서 GPO·레코드 읽기·보안 검증·Generate AC·복구를 순차 수행한 뒤, 마지막에 OUT 신호로 결과를 반환한다. 이 점이 구현 간 일관성을 확보하는 핵심이다.

## 슬라이드 24
최종 결과는 Outcome Parameter Set으로 정리된다. Approved, Online Request, Declined, End Application 같은 상태뿐 아니라 어떤 CVM이 수행되었는지, 영수증이 필요한지, RF 필드를 꺼야 하는지 같은 후속 행동 정보도 함께 담긴다. Terminal은 이 값을 받아 실제 사용자 경험으로 변환한다.

## 슬라이드 25
정리하면 Kernel 2는 비접촉 거래를 데이터 중심 상태 기계로 처리하는 표준 엔진이다. 구조 측면에서는 5개 프로세스와 Signal, TLV Database가 핵심이고, 기능 측면에서는 Mag-Stripe와 EMV 모드, ODA, Generate AC, SDS, IDS, Torn Recovery가 핵심이다. 이 틀을 잡으면 Mastercard 사양서를 세부 항목이 아니라 하나의 처리 체계로 읽을 수 있다.
