# Mastercard Kernel 2 구조 발표 대본

> EMV Contactless Book C-2 | Kernel 2 Specification v2.10
> 발표 대상: 결제 단말 개발자, 인증 엔지니어

---

## 슬라이드 1 — 표지

안녕하세요. 오늘은 Mastercard 컨택리스 결제의 핵심인 EMV Contactless Book C-2, 즉 Kernel 2의 구조에 대해 발표하겠습니다.

이 사양은 EMVCo가 2021년 3월에 발행한 버전 2.10을 기준으로 하며, 우리가 현재 개발하고 있는 결제 단말의 컨택리스 결제 처리 로직의 근거가 되는 문서입니다.

발표는 전체 아키텍처 구조부터 시작해서 데이터 흐름, 트랜잭션 모드, 보안 메커니즘, 그리고 실제 동작 예시 순서로 진행하겠습니다.

---

## 슬라이드 2 — 목차

발표는 크게 두 파트로 나뉩니다.

전반부에서는 시스템의 구조적인 측면을 다룹니다. 전체 POS 시스템 아키텍처, Reader를 구성하는 5개의 프로세스, 프로세스 간 통신을 위한 Signal 체계, 그리고 데이터베이스 구조까지 설명드립니다.

후반부에서는 기능적인 측면을 다룹니다. Mag-Stripe와 EMV 두 가지 트랜잭션 모드, Data Exchange와 Data Storage 메커니즘, Torn Transaction 복구, 그리고 C-APDU 명령어 구조와 상태 기계를 살펴본 후, 실제 동작 예시로 마무리하겠습니다.

---

## 슬라이드 3 — [섹션] 1. EMV 컨택리스 결제 개요

---

## 슬라이드 4 — EMV 컨택리스 결제란?

먼저 배경을 간단히 짚고 넘어가겠습니다.

EMVCo는 Europay, Mastercard, Visa가 공동으로 설립한 결제 표준 기구입니다. 컨택리스 결제는 ISO/IEC 14443 기반의 NFC 통신을 이용해 카드나 휴대폰을 단말기에 0.5초 이내에 태깅하는 방식입니다. 오프라인과 온라인 처리를 모두 지원하며, 최근에는 웨어러블 기기까지 확장되고 있습니다.

오른쪽 표에서 보시듯이, EMVCo 컨택리스 표준은 여러 권의 Book으로 구성되어 있습니다. Book A는 공통 아키텍처를 정의하고, Book B는 Entry Point를, Book C-2가 오늘 다루는 Mastercard의 Kernel 2입니다. Visa는 Kernel 3, Amex는 Kernel 4를 사용합니다.

하단을 보시면, Kernel 2는 두 가지 구현 옵션을 제공합니다. MAG 옵션은 마그네틱 스트라이프 방식의 데이터를 생성하고, EMV 옵션은 칩카드 방식의 암호학적 인증을 수행합니다. 두 옵션 모두 동일한 Book C-2 사양을 따르며, Kernel 2는 Mastercard 전용 커널입니다.

---

## 슬라이드 5 — Book C-2 범위

Book C-2의 구성을 보겠습니다.

왼쪽을 보시면, 이 문서는 크게 세 가지 핵심 내용을 다룹니다. Chapter 2의 일반 아키텍처에서는 POS 시스템 구성과 Reader 프로세스 역할을 정의합니다. Chapter 3의 Kernel 처리 기능에서는 실제 트랜잭션 처리의 모든 세부 사항을 다룹니다. Mag-Stripe/EMV 모드부터 Data Exchange, Data Storage, Torn Transaction 복구, Mobile 트랜잭션, Balance Reading, 그리고 Relay Resistance Protocol까지 포함합니다.

오른쪽에는 추가 구성 요소가 있습니다. Chapter 4는 TLV 데이터 조직, Chapter 5는 C-APDU 명령어 9종, Chapter 6은 16개 상태의 상태 기계를 정의합니다. Annex A에는 180개 이상의 데이터 오브젝트 사전이 있습니다.

중요한 설계 원칙이 있습니다. Kernel 요구사항은 구현 방식에 독립적입니다. Terminal과 Reader가 물리적으로 분리되어 있든 아니든, 내부 구현이 어떻든 관계없이, 외부에서 관찰되는 동작이 이 사양과 일치하면 됩니다.

---

## 슬라이드 6 — [섹션] 2. 전체 시스템 아키텍처

---

## 슬라이드 7 — POS 시스템 전체 구조

POS 시스템의 물리적 구성은 세 가지 형태가 가능합니다.

첫 번째는 통합형으로, 모든 기능이 단일 장치에 있는 형태입니다. 두 번째는 지능형 Reader로, Reader가 컨택리스 트랜잭션 대부분을 처리하고 결과만 Terminal에 전달하는 형태입니다. 세 번째는 Terminal과 투명 Reader의 조합으로, Reader는 RF 통신만 담당하고 Kernel 처리는 Terminal에서 수행하는 형태입니다.

Book C-2는 두 번째인 지능형 Reader 모델을 기준으로 설계되었습니다. 이 설계가 권장 사항이지만, 어떤 물리 구성을 사용하든 동작이 사양과 일치하면 됩니다.

아래 표에서 Terminal과 Reader의 역할 분리를 보실 수 있습니다. Terminal은 금액 입력 수신, CVM 처리, 온라인 권한 부여 요청, 영수증 출력 결정, 최종 승인/거부 결정을 담당합니다. Reader는 NFC RF 인터페이스 관리, 카드 앱 선택, 카드와 APDU 통신, EMV/Mag-Stripe 트랜잭션 처리, 그리고 결과를 OUT Signal로 반환하는 역할을 합니다.

---

## 슬라이드 8 — Reader 프로세스 구성 (5 Processes)

Reader는 독립적으로 실행되는 5개의 프로세스로 구성됩니다.

맨 위의 파란 박스들을 보시면, 왼쪽부터 Process M, S, P, D, K 순서입니다. 짙은 남색 박스가 오늘 발표의 핵심인 Process K, 즉 Kernel입니다.

각 프로세스의 역할을 아래 표에서 확인하실 수 있습니다. Process M은 전체 흐름을 조율합니다. Process S는 카드 앱과 커널을 선택합니다. Process P는 RF 인터페이스를 관리하고 C-APDU와 R-APDU를 중계합니다. Process D는 사용자 UI와 다국어 메시지를 관리합니다. 그리고 Process K, 즉 Kernel은 카드가 선택된 이후 결제 트랜잭션의 모든 처리를 담당합니다. 이것이 Book C-2가 정의하는 내용입니다.

신호 흐름은 Terminal에서 ACT Signal을 보내면 Process M을 거쳐 Process K로 전달되고, Process K가 Process P를 통해 카드와 APDU를 교환하며 트랜잭션을 처리하는 방식입니다.

---

## 슬라이드 9 — [섹션] 3. 프로세스 간 통신 (Signal 체계)

---

## 슬라이드 10 — Signal 기반 프로세스 간 통신

각 프로세스는 Signal을 통해 서로 통신합니다. Signal은 이름과 선택적 데이터 페이로드로 구성됩니다.

상단 흐름도를 보시면, Terminal이 ACT Signal을 보내면 Process M을 거쳐 Process K로 전달됩니다. Process K는 Process P에게 CA Signal로 카드 명령 전송을 요청하고, Process P가 카드와 실제 APDU를 교환한 후 RA Signal로 응답을 돌려줍니다. 트랜잭션이 완료되면 OUT Signal이 Terminal까지 역방향으로 전달됩니다.

중요한 것이 DEK와 DET Signal입니다. Kernel이 추가 데이터가 필요하거나 Terminal에 데이터를 전달할 때 DEK Signal을 사용하고, Terminal이 Kernel에 데이터를 제공하거나 제어를 위해 DET Signal을 사용합니다.

아래 표에서 각 Signal의 방향과 역할을 보실 수 있습니다. ACT는 트랜잭션 시작, OUT은 트랜잭션 결과 반환, STOP과 ABORT는 중단 요청입니다. MSG는 화면 표시 요청입니다.

---

## 슬라이드 11 — ACT / OUT Signal 상세 구조

ACT Signal과 OUT Signal의 내용을 좀 더 구체적으로 살펴보겠습니다.

왼쪽이 ACT Signal에 포함되는 주요 데이터입니다. 승인 금액, 캐시백 금액, 가맹점 코드, 국가코드, 통화코드, 거래 일시, 트랜잭션 유형, 그리고 예측 불가능한 4바이트 랜덤값인 Unpredictable Number가 포함됩니다. 선택적으로 Tags To Read와 Tags To Write도 포함될 수 있습니다.

오른쪽 위의 Outcome Parameter Set이 OUT Signal의 핵심입니다. Status 필드에 Approved, Online Request, Declined, End Application 중 하나가 담깁니다. CVM Performed에는 Online PIN, Confirm Code Verified, Signature, No CVM 중 적용된 인증 방법이 담깁니다.

오른쪽 아래에는 Data Record와 Discretionary Data가 있습니다. Data Record는 온라인 권한 부여나 정산에 사용되는 실제 결제 데이터입니다. Discretionary Data는 Kernel이 진단 목적으로 추가하는 정보로, Mag-Stripe 모드에서는 비트맵 형식의 Track 위치 데이터가, EMV 모드에서는 TVR 같은 진단 정보가 담깁니다.

---

## 슬라이드 12 — [섹션] 4. Reader Database 구조

---

## 슬라이드 13 — Reader Database — 영구·임시 데이터셋

Reader는 여러 데이터셋으로 구성된 데이터베이스를 유지합니다.

중요한 개념이 영구 데이터와 임시 데이터의 구분입니다. 영구 데이터는 여러 트랜잭션에 걸쳐 지속되고, 임시 데이터는 트랜잭션 시작 시 영구 데이터의 복사본으로 생성되어 해당 트랜잭션에만 사용됩니다.

5개의 박스를 보시면, Process M은 지원하는 AID별·트랜잭션 유형별 데이터와 국가코드, 통화코드 같은 일반 데이터를 영구 저장합니다. Process P는 RF 폴링 설정 데이터셋을 가집니다. Process D는 언어별 메시지 문자열을 영구 저장합니다. Process S는 지원 AID와 Kernel 매핑 데이터를 가집니다.

오늘 가장 중요한 것은 Process K, 즉 Kernel의 데이터베이스입니다. Kernel은 영구 데이터와 임시 데이터 모두를 사용합니다. 영구 데이터에는 TLV Database, CA 공개키 목록, 인증서 폐기 목록, Scratch Pad가 포함됩니다.

---

## 슬라이드 14 — Kernel 2 영구 데이터셋 상세

Kernel 영구 데이터셋의 구성 요소를 더 자세히 보겠습니다.

TLV Database는 AID별·트랜잭션 유형별로 독립된 데이터셋입니다. 약 180개의 데이터 오브젝트를 포함하며, 트랜잭션 시작 시 임시 복사본을 만들어 사용합니다.

CA Public Keys는 인증서 체인 검증에 사용하는 공개키 목록입니다. RID별로 최소 6개 키를 저장할 수 있으며, 동일한 RID를 가진 AID 간에 공유도 가능합니다.

오른쪽 위의 CRL은 Certification Revocation List, 인증서 폐기 목록입니다. CA Public Key Index와 Serial Number 조합으로 폐기된 발급사 인증서를 확인합니다.

오른쪽 아래의 Torn Transaction Log는 중단된 트랜잭션 복구를 위한 로그입니다. 나중에 자세히 설명드리겠지만, 카드가 트랜잭션 도중 RF 필드를 이탈했을 때 데이터를 여기에 저장해 뒀다가 카드가 다시 접촉하면 복구합니다.

데이터 카테고리별 쓰기 권한도 중요합니다. Kernel 전용, 카드 전용, Terminal 전용, Kernel과 Terminal 공유 등으로 구분되어 데이터 무결성을 보장합니다.

---

## 슬라이드 15 — [섹션] 5. Kernel 2 기능 및 구현·설정 옵션

---

## 슬라이드 16 — Kernel 2 주요 기능 요약

Kernel 2가 제공하는 기능 전체를 한눈에 보여주는 슬라이드입니다.

표를 보시면 총 9가지 주요 기능이 있습니다.

트랜잭션 모드는 Mag-Stripe와 EMV 두 가지를 지원합니다. CVM은 카드소지자 인증으로 Online PIN, No CVM 등을 처리합니다. Data Exchange는 DEK/DET Signal로 Terminal과 Kernel이 실시간으로 데이터를 주고받는 메커니즘입니다. Data Storage는 카드에 데이터를 읽고 쓰는 확장 기능입니다. Torn Transaction Recovery는 트랜잭션 중단 시 복구 기능입니다. Mobile 트랜잭션은 휴대폰의 자체 CVM 처리를 지원합니다. Balance Reading은 카드의 오프라인 잔액 조회입니다. Relay Resistance Protocol은 릴레이 공격을 방어합니다. 마지막으로 Non-CDA 최적화는 CDA가 필요 없을 때 레코드 읽기를 최소화합니다.

이 기능들은 각각 Chapter 3의 별도 섹션에서 상세히 정의되어 있습니다.

---

## 슬라이드 17 — 구현 옵션 & 설정 옵션

Kernel 2는 두 가지 구현 옵션과 여덟 가지 설정 옵션을 제공합니다.

왼쪽 구현 옵션부터 보겠습니다. MAG 옵션은 Track 1/2 형식의 데이터를 생성하는 마그네틱 스트라이프 방식입니다. 처리 구조가 비교적 단순합니다. EMV 옵션은 SDA, DDA, CDA 같은 오프라인 데이터 인증과 ARQC/TC/AAC 암호학적 인증을 수행하며, Data Storage 기능도 포함합니다.

오른쪽 설정 옵션은 Kernel Configuration 비트 플래그로 활성화합니다. CDA 지원, Mag-Stripe 모드, SDS, IDS, Balance Reading, Relay Resistance, 하이브리드 단말 여부 등을 설정할 수 있습니다.

하단에서 중요한 규칙을 확인하실 수 있습니다. 각 설정 옵션은 활성화 조건이 있습니다. 예를 들어 CDA는 Kernel Configuration 비트도 활성화되어 있어야 하고, 카드의 AIP 비트도 설정되어 있어야 합니다. 또한 모든 설정 옵션이 모든 구현 옵션에 적용되지는 않습니다. MAG 옵션에서는 SDS, IDS, CDA를 사용할 수 없습니다.

---

## 슬라이드 18 — [섹션] 6. Kernel Database (TLV 구조)

---

## 슬라이드 19 — TLV Database 원칙 및 서비스

Kernel의 데이터 관리 방식을 이해하는 것이 중요합니다.

Kernel은 모든 데이터를 TLV, 즉 Tag-Length-Value 형식으로 인코딩된 데이터베이스에서 관리합니다. TLV Database는 Kernel 인스턴스화 시 영구 데이터셋의 복사본으로 초기화되고, 트랜잭션 중에 카드와 Terminal에서 수신한 데이터로 업데이트됩니다.

왼쪽에 TLV Database의 특성이 있습니다. 각 AID와 트랜잭션 유형별로 독립된 인스턴스가 생성됩니다. 길이가 0인 항목도 '존재'하는 것으로 취급되는데, 이것이 Data Exchange 요청 트리거 메커니즘에 활용됩니다.

오른쪽에는 TLV Database 서비스 API가 있습니다. Kernel 코드는 이 API를 통해서만 데이터에 접근합니다. IsKnown은 태그가 데이터 사전에 정의되어 있는지, IsPresent는 현재 DB에 값이 있는지, IsEmpty는 길이가 0인지를 확인합니다. GetValue와 GetTLV는 값을 가져오고, Store는 값을 저장합니다.

하단의 DOL 처리도 중요합니다. DOL은 Data Object List로, 특정 명령에 전달할 데이터 목록을 정의합니다. PDOL은 GET PROCESSING OPTIONS 명령에, CDOL1/CDOL2는 GENERATE AC 명령에, DRDOL은 RECOVER AC 명령에 사용됩니다.

---

## 슬라이드 20 — Working Variables & Configuration Data

TLV Database 외에 Kernel이 사용하는 Working Variables를 보겠습니다.

Working Variables는 두 가지로 구분됩니다. 로컬 변수는 특정 상태 전이 처리 내에서만 유효하며 데이터 사전에 태그가 없습니다. Missing PDOL Data Flag나 Sync Data가 여기에 해당합니다. 글로벌 변수는 Kernel 프로세스 수명과 동일하게 유지되며, Active AFL이나 AC Type 같은 변수가 이에 해당합니다.

오른쪽의 Torn Transaction Log는 중단 트랜잭션 추적용입니다. 각 항목에는 AID와 필수 데이터 오브젝트가 저장됩니다. 카드가 재접촉하면 UDOL 기반으로 동일 카드인지 확인합니다. 로그가 꽉 찼을 때는 가장 오래된 항목을 삭제하고, CLEAN Signal로 만료된 항목을 정기적으로 정리합니다.

하단의 Configuration Data는 Kernel 인스턴스화 시 반드시 초기화되어야 하는 데이터입니다. 이 데이터가 없으면 Kernel이 동작하지 않습니다. Reader CVM Required Limit는 CVM을 적용할 금액 한도, Contactless Transaction Limit는 컨택리스 허용 최대 금액, Floor Limit는 오프라인 승인 가능 한도입니다. Kernel Configuration은 기능 활성화 8비트 플래그이고, Timeout Value는 카드 응답 대기 최대 시간입니다.

---

## 슬라이드 21 — [섹션] 7. 트랜잭션 모드

---

## 슬라이드 22 — Mag-Stripe 모드 vs EMV 모드

이제 실제 트랜잭션 처리를 살펴보겠습니다.

GET PROCESSING OPTIONS 명령을 카드에 전송하면 AIP, Application Interchange Profile을 수신합니다. 이 AIP의 비트 설정에 따라 Mag-Stripe 모드 또는 EMV 모드로 분기됩니다.

왼쪽 Mag-Stripe 모드를 보겠습니다. AIP에 Mag-Stripe 비트가 설정되어 있으면 이 경로를 따릅니다. READ RECORD로 Track 1/2 데이터를 수집한 뒤, 예측 불가능한 숫자인 UN을 생성하고, 카드에 COMPUTE CRYPTOGRAPHIC CHECKSUM 명령을 보내 CVC3 암호문을 받습니다. 이 CVC3를 Track 2 데이터의 Discretionary Data 필드에 삽입한 후 온라인 승인 요청을 합니다. Mag-Stripe 모드는 항상 온라인 승인을 요청합니다.

오른쪽 EMV 모드를 보겠습니다. AIP 확인 후 어떤 형태의 오프라인 데이터 인증을 수행할지 결정합니다. SDA, DDA, CDA 중 하나입니다. 그런 다음 READ RECORD로 카드 레코드를 읽고, Terminal Risk Management를 수행하여 GENERATE AC 명령으로 암호문을 요청합니다. 카드의 응답 유형에 따라 온라인 요청, 오프라인 승인, 거부 중 하나로 결론이 납니다.

---

## 슬라이드 23 — EMV 모드 — 오프라인 데이터 인증 (ODA)

EMV 모드의 핵심인 오프라인 데이터 인증을 자세히 보겠습니다.

표에서 세 가지 ODA 방식을 비교하고 있습니다.

SDA, Static Data Authentication은 정적 서명을 검증합니다. 카드가 개인화 시 서명된 SDAD를 레코드에 담아두고, Kernel이 이를 검증합니다. 보안 수준이 낮아 복제 공격에 취약합니다.

DDA, Dynamic Data Authentication은 카드가 매 트랜잭션마다 동적 서명을 생성합니다. INTERNAL AUTHENTICATE 명령을 통해 ICC가 서명하고, Kernel이 검증합니다.

CDA, Combined DDA/Generate AC는 가장 강력한 방식입니다. GENERATE AC 명령과 동적 서명이 통합되어, AC와 SDAD를 하나의 응답으로 받습니다. 추가 명령이 필요 없어 속도도 빠릅니다.

왼쪽 하단에 공개키 인증서 체인이 있습니다. CA의 공개키로 Issuer 인증서를 검증하고, Issuer 공개키로 ICC 인증서를 검증하는 계층 구조입니다. CRL로 폐기된 인증서는 사전에 걸러냅니다.

오른쪽 하단에는 Terminal Risk Management입니다. TVR 비트맵에 각 처리 단계의 오류나 경고를 기록하고, Terminal Action Code와 Issuer Action Code를 기반으로 최종 AC 유형을 결정합니다.

---

## 슬라이드 24 — [섹션] 8. Data Exchange 메커니즘

---

## 슬라이드 25 — Data Exchange — DEK / DET Signal

Data Exchange는 Kernel과 Terminal이 트랜잭션 중에 동적으로 데이터를 주고받는 메커니즘입니다.

왼쪽을 보시면, DEK Signal은 Kernel에서 Terminal로 전송됩니다. Data Needed 필드에 Kernel이 필요한 태그 목록을 담고, Data To Send 필드에 Terminal에 전달할 TLV 데이터를 담습니다. Database ID와 Session ID로 어느 Kernel의 어느 세션에 대한 요청인지 식별합니다. DET Signal은 Terminal에서 Kernel로, Kernel Database를 업데이트하거나 추가 Tags To Read/Write를 제공합니다.

오른쪽의 Tags To Read와 Tags To Write가 핵심입니다. Tags To Read는 트랜잭션 중 카드에서 읽어야 할 태그 목록입니다. Tags To Write Before Gen AC는 GENERATE AC 이전에 카드에 써야 할 데이터, After Gen AC는 이후에 써야 할 데이터입니다. 이 값들의 길이가 0이면 Kernel이 DEK Signal로 Terminal에 값을 요청합니다.

하단에서 보시듯이 이 메커니즘은 Terminal이 트랜잭션 속도를 조절하는 데도 활용됩니다. 필요한 데이터를 미리 제공하면 처리가 빨라지고, 의도적으로 늦게 제공하면 Kernel의 처리를 잠시 멈출 수 있습니다.

---

## 슬라이드 26 — [섹션] 9. Data Storage (SDS / IDS)

---

## 슬라이드 27 — Data Storage 개요 — SDS vs IDS

Data Storage는 EMV 모드 트랜잭션에서 카드를 메모 패드처럼 활용하는 확장 기능입니다. Mag-Stripe 모드에는 없습니다.

왼쪽의 SDS, Standalone Data Storage를 보겠습니다. GET DATA와 PUT DATA라는 전용 명령을 사용합니다. 태그 범위 '9F70'부터 '9F79'까지 비결제 데이터를 저장합니다. '9F70'~'9F74'는 보안 메시지가 필요하고, '9F75'~'9F79'는 보안 메시지 없이 쓸 수 있습니다. 기존 결제 명령과 완전히 독립되어 구현이 단순합니다.

오른쪽의 IDS, Integrated Data Storage는 기존 결제 명령에 통합됩니다. GPO 응답으로 카드에서 데이터를 읽고, GENERATE AC 명령의 데이터 필드에 쓰기 데이터를 통합합니다. 별도 명령이 추가되지 않습니다. AC Type 다운그레이드 메커니즘도 있습니다. IDS에서 원하는 AC Type이 Kernel의 AC Type보다 낮으면, Kernel이 자신의 AC Type을 낮춰서 맞춥니다.

---

## 슬라이드 28 — IDS 보안 설계

IDS는 데이터 무결성과 보안을 위한 여러 메커니즘을 갖추고 있습니다.

왼쪽에서 세 가지 보안 속성을 설명합니다.

인증성은 Terminal이 카드에 저장된 DS ODS Card 데이터에 MAC을 추가하여 제3자의 위변조를 방지합니다.

복제 방지는 DS ID, 즉 인증된 카드 식별자와 DS Unpredictable Number, 카드가 생성한 챌린지 값을 조합하여 카드 복제를 감지합니다.

쓰기 제어는 원웨이 함수 기반으로 합니다. 카드 개인화 시 DS Input의 해시값을 저장해두고, 쓰기 요청 시 제출된 해시와 비교하여 쓰기 권한을 검증합니다.

오른쪽에는 DS Summary 검증 절차가 있습니다. 카드에서 DS Summary 1을 GPO 응답으로 읽고, Kernel이 OWHF2 해시 함수를 적용하여 DS Summary 2를 계산합니다. 카드는 GENERATE AC 응답에 DS Summary 3을 포함시키는데, DS Summary 2와 DS Summary 3이 일치하면 IDS 인증에 성공합니다. OWHF2는 AES 기반 단방향 해시 함수로 Chapter 8에서 정의합니다.

---

## 슬라이드 29 — [섹션] 10. Torn Transaction Recovery

---

## 슬라이드 30 — Torn Transaction Recovery — 카드 이탈 복구

Torn Transaction, 즉 찢어진 트랜잭션이란 카드소지자가 트랜잭션 완료 전에 카드를 RF 필드에서 제거하는 상황을 말합니다.

이것이 문제가 되는 이유는, 특히 오프라인 잔액이 있는 선불 카드의 경우 잔액이 이미 차감되었을 수 있는데, 카드가 이탈했으니 트랜잭션도 완료되지 않습니다. 이중 차감 방지를 위해 복구 메커니즘이 필요합니다.

왼쪽에서 개요를 보시면, DRDOL, Data Recovery Data Object List의 존재 여부로 카드가 복구를 지원하는지 확인합니다. DRDOL이 있고 길이가 0보다 크면 복구를 지원합니다.

오른쪽에 복구 절차가 있습니다. Kernel이 READ RECORD 이후 Torn Transaction Log에 항목을 생성합니다. 카드 이탈 감지 시 재접촉 UI를 요청합니다. 카드가 재접촉하면 GPO를 실행한 후 Log에서 동일 카드를 UDOL 매칭으로 확인합니다. 매칭이 성공하면 RECOVER AC 명령을 전송합니다. 카드가 이전 트랜잭션의 AC를 반환하면 복구가 완료됩니다. 처리 완료 후 Log 항목을 삭제하고 OUT Signal을 반환합니다.

---

## 슬라이드 31 — [섹션] 11. Mobile · Balance Reading · Relay Resistance

---

## 슬라이드 32 — Mobile 트랜잭션 · Balance Reading · Relay Resistance Protocol

이번 슬라이드에서는 세 가지 부가 기능을 함께 설명드립니다.

왼쪽의 Mobile 트랜잭션입니다. 휴대폰은 카드와 달리 자체적으로 카드소지자를 인증할 수 있습니다. 이를 CDCVM, Consumer Device CVM이라 합니다. Kernel은 카드에서 CDCVM 수행 여부를 확인하고, CDCVM이 성공했으면 CVM을 No CVM으로 처리합니다. 미지원 기기면 일반 CVM 처리를 합니다.

가운데 Balance Reading입니다. 선불 카드나 일부 카드는 오프라인 잔액을 가집니다. Application Capabilities Information에서 지원 여부를 확인하고, GET DATA 명령으로 '9F79' 태그의 잔액을 조회합니다. GENERATE AC 이전 또는 이후에 조회할 수 있으며, Terminal 화면이나 영수증으로 고객에게 보여줍니다. 이는 선택적 설정 옵션입니다.

오른쪽 Relay Resistance Protocol입니다. 릴레이 공격은 위조 단말이 멀리 있는 진짜 단말에 트랜잭션을 중계하는 공격입니다. 이를 막기 위해 RF 전달 지연 시간을 측정합니다. EXCHANGE RELAY RESISTANCE DATA 명령으로 단말 난수와 타임스탬프를 전송하고 카드의 처리 시간을 측정합니다. 카드가 지정한 Min/Max 시간 범위 내에 있는지 확인하여, 초과하면 TVR에 RRP 실패 비트를 설정합니다.

---

## 슬라이드 33 — [섹션] 12. C-APDU 명령어 구조

---

## 슬라이드 34 — C-APDU 명령어 목록

Kernel 2가 카드에 전송하는 명령어를 정리했습니다.

CLA 바이트는 '80'이 독점 클래스를 나타내며, READ RECORD만 '00'을 사용합니다.

표에서 9가지 명령어를 확인하실 수 있습니다.

Compute Cryptographic Checksum은 Mag-Stripe 모드에서 CVC3 암호문을 계산합니다. Exchange Relay Resistance Data는 RRP 타이밍 측정에 사용합니다. Generate AC는 트랜잭션의 핵심으로 ARQC, TC, AAC 암호문을 생성합니다. Get Data는 단일 데이터 오브젝트를 조회하며 SDS와 Balance Reading에 사용합니다. Get Processing Options는 트랜잭션의 시작으로 AIP와 AFL을 받습니다. Put Data는 SDS에서 카드에 데이터를 씁니다. Read Record는 AFL이 가리키는 레코드를 읽습니다. Recover AC는 중단된 트랜잭션의 AC를 복구합니다.

---

## 슬라이드 35 — 핵심 명령어 — GET PROCESSING OPTIONS & GENERATE AC

가장 중요한 두 명령어를 자세히 보겠습니다.

왼쪽 GET PROCESSING OPTIONS입니다. PDOL Related Data를 데이터 필드에 담아 전송합니다. PDOL은 카드가 Terminal에게 요청하는 데이터 목록입니다. 카드는 응답으로 AIP와 AFL을 반환합니다. AIP의 주요 비트를 체크해야 합니다. SDA, DDA, CDA 지원 여부, Mag-Stripe 모드 지원, RRP 지원, IDS 지원 여부가 여기 담겨 있습니다. AFL은 어느 SFI의 몇 번째 레코드를 읽어야 하는지를 지시하는 파일 로케이터입니다.

오른쪽 GENERATE AC입니다. Reference Control Parameter, RCP의 P1 바이트가 중요합니다. '40'은 TC, 오프라인 승인, '80'은 ARQC, 온라인 요청, '00'은 AAC, 거부를 나타냅니다. 여기에 비트 4를 설정하면 CDA를 함께 요청합니다. 데이터 필드는 CDOL1 Related Data를 담습니다. 응답은 두 가지 형식이 있습니다. Format 1은 ATC, AC 암호문, IAD를 포함하며 CDA가 없을 때 사용합니다. Format 2는 SDAD 동적 서명을 추가로 포함하며 CDA를 사용할 때 사용합니다.

온라인 승인 후 Issuer 응답을 받으면 2차 GENERATE AC를 보냅니다. 이때는 CDOL2 데이터를 사용하며 Issuer Script 실행 결과도 포함됩니다.

---

## 슬라이드 36 — [섹션] 13. Kernel 상태 기계 (State Machine)

---

## 슬라이드 37 — Kernel 2 상태 기계 — 16개 상태

Kernel의 처리는 상태 기계로 명세됩니다. 외부 Signal을 수신하면 상태가 전이되고, 각 상태에서 특정 처리를 수행한 후 다음 상태로 이동합니다.

16개의 상태를 표로 보여드립니다.

State 1 Idle은 초기 상태로 ACT, STOP, CLEAN Signal을 대기합니다. State 2는 GPO 응답을 대기합니다. State 3와 R1은 READ RECORD 루프의 공통 처리입니다. State 4는 EMV 모드 레코드 읽기 응답, State 5는 GET DATA 응답, State 6은 GENERATE AC 이전 SDS 쓰기, State 7은 재시작 처리입니다. State 8은 Exchange Relay Resistance Data 응답입니다.

State 9와 10이 중요합니다. State 9는 1차 GENERATE AC 응답 처리와 CDA 서명 검증을 합니다. State 10은 RECOVER AC 응답으로 Torn Transaction 복구를 처리합니다. State 11은 State 9와 10의 공통 처리로 최종 Outcome을 결정합니다. State 12~16은 Balance Reading, 2차 Generate AC, IDS 쓰기 등 부가 기능 처리입니다.

---

## 슬라이드 38 — State 1 — Idle (트랜잭션 시작)

State 1의 동작을 상세히 보겠습니다.

왼쪽 흐름도를 보시면, ACT Signal을 수신하면서 시작됩니다. Sync Data를 파싱하고 Kernel DB를 초기화합니다. PDOL 존재 여부를 확인하여 PDOL Related Data를 구성하고, GET PROCESSING OPTIONS 명령을 전송하여 State 2로 전이합니다.

가운데의 분기 처리를 보겠습니다. STOP Signal이 오면 Status를 END APPLICATION으로 설정하고 즉시 OUT Signal을 반환합니다. CLEAN Signal이 오면 Torn Transaction Log에서 만료된 항목을 찾아 삭제하고, 만료 항목이 있으면 Discretionary Data에 포함하여 OUT Signal을 반환합니다.

오른쪽의 처리 상세입니다. PDOL이 없으면 PDOL Related Data를 '8300'으로 설정합니다. PDOL이 있으면 각 태그값을 DB에서 조회하고, 없는 항목은 빈 값으로 대체하면서 Missing PDOL Data Flag를 true로 설정합니다. Tags To Read Yet 버퍼도 초기화됩니다. 설정된 Tags To Read 목록을 복사해두고 트랜잭션 중 하나씩 수집합니다.

---

## 슬라이드 39 — State 9/10 — Generate AC 응답 처리

State 9와 10의 처리를 보겠습니다.

왼쪽 State 9는 1차 GENERATE AC 응답을 처리합니다. 먼저 응답 형식을 파싱합니다. Format 1은 Tag 없이 연속된 값이고, Format 2는 TLV 인코딩입니다. CID, Cryptogram Information Data에서 AC 유형을 확인합니다. 비트 7-6이 00이면 AAC, 01이면 TC, 10이면 ARQC입니다. CDA가 활성화되어 있으면 SDAD 서명을 ICC 공개키로 검증하고 터미널 데이터 해시를 확인합니다. 검증 실패 시 CDA Failed 플래그를 설정합니다. Torn Transaction이었다면 Log에서 항목을 삭제합니다.

오른쪽 State 11, 공통 처리에서 최종 Outcome을 결정합니다. AAC이면 Outcome을 DECLINED로 설정하고 'Not Authorised' 메시지를 UI에 요청합니다. TC이면 APPROVED로 설정하고 영수증 필요 여부를 판단합니다. ARQC이면 ONLINE_REQUEST로 설정하고 Data Record를 생성한 뒤 Terminal이 온라인 응답을 가져올 때까지 대기합니다. 최종적으로 Outcome Parameter Set, Data Record, Discretionary Data를 구성하여 OUT Signal로 Terminal에 반환합니다.

---

## 슬라이드 40 — [섹션] 14. 동작 예시

---

## 슬라이드 41 — 동작 예시 ① — 일반 구매 (EMV 모드, 오프라인 승인)

이제 실제 동작 예시를 통해 전체 흐름을 정리해 보겠습니다.

첫 번째는 소액 구매에서 TC, 오프라인 승인이 발행되는 가장 일반적인 흐름입니다.

표의 색상으로 Actor를 구분했습니다. 주황색은 Terminal, 파란색은 Kernel 내부 처리, 초록색은 카드와의 통신입니다.

Terminal이 ACT Signal을 전송합니다. Kernel은 State 1에서 PDOL을 구성하고 GPO 명령을 전송합니다. 카드로부터 AIP와 AFL을 수신합니다. AIP에 EMV 모드와 CDA 지원 비트가 있습니다. Kernel은 State 2에서 3으로 전이하며 READ RECORD를 반복합니다. 세 개의 레코드를 읽으면 Issuer 인증서, ICC 인증서, TVR 관련 데이터를 수집합니다. ODA를 수행하여 CA→Issuer→ICC 인증서 체인을 검증합니다. Terminal Risk Management에서 금액이 Floor Limit 이하이므로 TC를 결정합니다. GENERATE AC를 RCP='40'으로 전송하여 TC를 요청합니다. 카드에서 Format 2 응답으로 TC와 SDAD 서명을 받습니다. CDA 검증 성공 후 State 11에서 Outcome을 APPROVED로 설정합니다. 마지막으로 OUT Signal에 Approved Outcome과 Data Record를 담아 Terminal에 반환합니다.

---

## 슬라이드 42 — 동작 예시 ② — 고액 구매 (EMV 모드, 온라인 승인)

두 번째는 고액 거래에서 ARQC가 발행되어 온라인 승인을 받는 흐름입니다.

Terminal이 500,000원 구매로 ACT Signal을 보냅니다. Contactless Transaction Limit를 초과하거나 TAC/IAC 분석 결과에 따라 ARQC가 결정됩니다.

Kernel은 GENERATE AC를 RCP='80'으로 전송하여 ARQC를 요청합니다. 카드에서 ARQC와 IAD, ATC를 수신합니다. State 11에서 Outcome을 ONLINE_REQUEST로 설정하고 Data Record를 생성합니다. OUT Signal로 ARQC가 포함된 Data Record를 Terminal에 전달합니다. Terminal이 Issuer에게 온라인으로 전송하면 Issuer가 ARPC와 선택적으로 Issuer Script를 반환합니다. Terminal은 DET Signal로 온라인 응답 데이터를 Kernel에 제공합니다. Kernel이 2차 GENERATE AC를 RCP='40'으로 전송하여 TC를 요청합니다. 카드가 TC를 반환하면 최종 OUT Signal로 APPROVED를 Terminal에 반환합니다.

---

## 슬라이드 43 — 동작 예시 ③ — Torn Transaction Recovery

세 번째는 카드소지자가 트랜잭션 중에 카드를 빼버린 후 다시 접촉하여 복구하는 시나리오입니다.

1단계를 보겠습니다. Kernel이 READ RECORD까지는 정상적으로 진행하고, GENERATE AC 명령을 전송합니다. 이때 카드가 RF 필드를 이탈하여 응답이 없습니다. Kernel은 이 시점에 이미 Torn Transaction Log에 현재 트랜잭션 데이터를 기록했습니다. MSG Signal로 재접촉 요청 메시지를 Display에 요청하고, Outcome을 TRY_AGAIN으로 설정하여 OUT Signal을 반환합니다.

2단계에서 카드가 다시 접촉하면, Process M이 트랜잭션을 재시작합니다. AID 선택과 GPO를 다시 수행합니다. GPO 응답에서 DRDOL이 있음을 확인하여 복구 지원 카드임을 인식합니다. Torn Transaction Log를 검색하여 UDOL 기반으로 동일 카드인지 확인합니다. 매칭이 성공하면 RECOVER AC 명령을 전송합니다. 카드가 이전 트랜잭션의 AC를 반환하면 복구가 완료됩니다. Log에서 항목을 삭제하고 정상적으로 OUT Signal을 반환합니다. 카드의 오프라인 잔액이 이중 차감되지 않습니다.

---

## 슬라이드 44 — 동작 예시 ④ — Mag-Stripe 모드 트랜잭션

네 번째는 Mag-Stripe 모드 트랜잭션 흐름입니다.

Terminal이 ACT Signal을 전송합니다. Kernel이 PDOL을 구성하고 GET PROCESSING OPTIONS를 전송합니다. 카드에서 GPO 응답이 오는데, AIP에 Mag-Stripe 모드 비트가 설정되어 있습니다. Kernel이 State 2에서 Mag-Stripe 모드로 분기합니다. READ RECORD로 Track 1과 Track 2 데이터를 읽습니다. Unpredictable Number를 10진수 형태로 생성합니다. COMPUTE CRYPTOGRAPHIC CHECKSUM 명령을 전송하고, 카드에서 CVC3 암호문, ATC, nUN, nq를 받습니다. nUN과 nq는 CVC3를 Track 2의 어느 위치에 삽입할지를 나타내는 비트맵입니다. Kernel이 Track 데이터에 CVC3를 삽입하여 조립합니다. State 11에서 Outcome을 ONLINE_REQUEST로 설정합니다. Mag-Stripe 모드는 항상 온라인 승인을 요청합니다. OUT Signal에 Track 형식의 Data Record를 담아 Terminal에 반환합니다.

---

## 슬라이드 45 — Outcome Parameter Set — 트랜잭션 결과 구조

Outcome Parameter Set의 각 필드를 상세히 보겠습니다.

Status는 Approved, Online Request, Declined, End Application 중 하나로 최종 트랜잭션 상태를 나타냅니다.

Start는 재시작 유형으로 No는 정상 종료, A/B/C는 다른 파라미터로 재시도를 의미합니다.

CVM Performed는 적용된 인증 방법입니다. Online PIN, Confirmation Code Verified, Obtain Signature, No CVM, NA 중 하나입니다.

Receipt는 영수증 발행이 필요한지를 나타냅니다.

Data Record와 Discretionary Data는 해당 데이터의 포함 여부를 나타냅니다.

Field Off Request는 RF 필드를 비활성화할지 여부이고, Removal Timeout은 카드 제거 대기 시간을 밀리초 단위로 나타냅니다.

Terminal은 이 Outcome Parameter Set을 파싱하여 다음 동작을 결정합니다. 화면 표시, 영수증 출력, 온라인 전송 여부 등이 여기서 결정됩니다.

---

## 슬라이드 46 — 보안 알고리즘 — OWHF2 & OWHF2AES

Chapter 8에서 정의하는 두 가지 보안 알고리즘입니다.

왼쪽 OWHF2는 3DES 기반 단방향 해시 함수입니다. 입력으로 DS Input 카드 개인화 데이터를 받아 16바이트 해시값을 출력합니다. IDS의 DS Summary 2를 계산하는 데 사용됩니다. 입력의 첫 8바이트와 두 번째 8바이트로 키를 구성하여 TDES 암호화를 적용한 후 XOR 연산을 수행합니다.

오른쪽 OWHF2AES는 AES-128 기반의 더 강력한 버전입니다. 동일한 역할을 하지만 더 높은 암호 강도를 제공합니다.

Unpredictable Number 생성도 여기서 다룹니다. Mag-Stripe 모드에서는 UN(Numeric)이라 하여 10진수 형태로 생성하고, EMV 모드에서는 4바이트 이진수 형태로 생성합니다. RRP에서 사용하는 Terminal 난수도 포함됩니다.

---

## 슬라이드 47 — Kernel 2 구조 종합 요약

지금까지 살펴본 Kernel 2의 구조를 정리합니다.

왼쪽에서 세 가지 핵심 개념을 정리했습니다.

아키텍처 측면에서는 5개 프로세스가 독립 실행되며 Signal 기반으로 느슨하게 결합되어 있습니다. 모든 데이터는 TLV Database 중심으로 관리됩니다.

트랜잭션 모드 측면에서는 Mag-Stripe는 CVC3 암호문을 사용하고 항상 온라인 전송합니다. EMV는 공개키 기반 ODA를 수행하고 오프라인 또는 온라인 승인이 가능합니다.

데이터 관리 측면에서는 DEK/DET Signal로 Terminal과 Kernel이 실시간으로 데이터를 교환합니다. SDS는 독립 명령을 사용하고, IDS는 기존 결제 명령에 통합됩니다.

오른쪽에서 복구, 보안, 확장 기능을 정리했습니다.

복구와 안전성으로는 Torn Transaction Recovery로 중단 트랜잭션을 복구하고, RRP로 릴레이 공격을 방어합니다. IDS의 OWHF2/MAC으로 데이터 무결성을 보장합니다.

확장 기능으로는 Mobile CDCVM, Balance Reading, Non-CDA 최적화가 있습니다.

상태 기계 관점에서는 16개의 상태가 Signal에 의해 구동되며, 카드 이탈, 거부, 승인 등 모든 경우를 처리하고 Outcome Parameter Set으로 결과를 Terminal에 반환합니다.

Kernel 2는 Mastercard 컨택리스 결제의 모든 처리 로직을 완전히 정의하는 핵심 사양입니다. 이 사양을 정확히 구현하는 것이 EMVCo 인증의 기반이 됩니다.

---

## 슬라이드 48 — Q & A

이상으로 Mastercard Kernel 2 구조 분석 발표를 마치겠습니다.

질문이 있으시면 말씀해 주세요.

---

*참고: EMV® Contactless Specifications for Payment Systems, Book C-2, Kernel 2 Specification v2.10, March 2021. © 2011-2021 EMVCo, LLC.*
